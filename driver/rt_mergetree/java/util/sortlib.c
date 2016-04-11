#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include "java_util_Arrays.h"
#include <jni.h>

#define LOOPSIZE 128

int32_t *array;
unsigned char *data_to_fpga;
unsigned char *data_from_fpga;
int data_size = 0;
int item_size = 0;
int sortLength = 0;
int mround = 0;
int fpga_write_fd;
int loopsize = 2;
int finish = 0;

long long current_timestamp()
{
    struct timeval te;
    gettimeofday(&te, NULL);    // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;    // caculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
//WRITE STREAM
//------------------------------------------------------------------------
void allwrite(int fd, unsigned char *buf, int len)
{
    int sent = 0;
    int send_size = 0;
    int rc;

    while (sent < len) {
        rc = write(fd, buf + sent, len - sent);

        if ((rc < 0) && (errno == EINTR)){
        	printf("here (?!)\n");
            continue;
        }

        if (rc < 0) {
            printf("allwrite() failed to write");
            continue;
        }

        if (rc == 0) {
            printf("Reached write EOF (?!)\n");
            break;
        }

        sent += rc;
    }
}

void *writeStream(void *param)
{
    int fd, fr, rc, i;
    unsigned char buf[4];

    printf("in writeStream for %d: %lld\n", sortLength, current_timestamp());

    buf[3] = (sortLength >> 24) & 0xFF;
    buf[2] = (sortLength >> 16) & 0xFF;
    buf[1] = (sortLength >> 8) & 0xFF;
    buf[0] = sortLength & 0xFF;
    allwrite(fpga_write_fd, buf, 4);

    allwrite(fpga_write_fd, data_to_fpga, data_size);

    printf("out writeStream for %d: %lld\n", sortLength, current_timestamp());
}

void writeFinish()
{
    int fd, fr, rc, i;
    unsigned char buf[4];

    printf("in writeFinish for %d: %lld\n", sortLength, current_timestamp());

    buf[3] = 0xFF;
    buf[2] = 0xFF;
    buf[1] = 0xFF;
    buf[0] = 0xFF;
    allwrite(fpga_write_fd, buf, 4);

    printf("out writeFinish for %d: %lld\n", sortLength, current_timestamp());
}

//------------------------------------------------------------------------

void readStream()
{
    int i, j, k, l, r, cnt;
    int fd, rc;
    int32_t tmp;
    unsigned char buf[128];

    for (r = 0; r < mround; r++) {
    	fd = open("/dev/xillybus_read_32", O_RDONLY);

	    if (fd < 0) {
	        if (errno == ENODEV)
	            fprintf(stderr, "(Maybe xillybus_read_32 a write-only file?)\n");
	        perror("Failed to open devfile");
	    }

    	cnt = 0;
        printf("in readStream for %d: %lld\n", sortLength, current_timestamp());
//bottom 1 line comment
//        fprintf(stderr, "C: %d, T:%d\n", cnt, data_size);

        while (cnt < data_size) {
            rc = read(fd, buf, sizeof(buf));
            if ((rc < 0) && (errno == EINTR))
                continue;
            if (rc < 0)
                perror("allread() failed to read");
            if (rc == 0)
                fprintf(stderr, "Reached read EOF.\n");

            for (i = 0; i < rc; i++) {
                data_from_fpga[cnt] = buf[i];
                cnt++;
            }
//bottom 1 line was comment
//            fprintf(stderr, "r: %d, C: %d, T:%d\n", r, cnt, data_size);
        }
        close(fd);

    	writeFinish();
        
        printf("end readStream for %d: %lld\n", sortLength, current_timestamp());
        printf("\n");
        printf("r: %d\n", r);

	if (r < (mround - 1)) {
            //prepare data for next round
            printf("in prepare for %d: %lld\n", sortLength, current_timestamp());
            sortLength = sortLength * loopsize; //change loop size
	    printf("sortLength %d\n", sortLength);
            int group_size = sortLength * loopsize * 4; //change loop size
            int group_num = data_size / group_size;
            cnt = 0;
            for (i = 0; i < group_num; i++) {
                for (j = 0; j < sortLength * 4; j = j + 4) {
		    for (l = 0; l < loopsize; l++){
                    	for (k = 0; k < 4; k++) {
                        	data_to_fpga[cnt] = data_from_fpga[i * group_size + j + sortLength * 4 * l + k];
                        	cnt++;
                    	}
		    }
                }
            }
            printf("end prepare for %d: %lld\n", sortLength, current_timestamp());

            pthread_t write_thread;
	        if (pthread_create(&write_thread, NULL, writeStream, NULL)) {
	            fprintf(stderr, "Error creating thread\n");
	        }
        } else {
            printf("last round %d: %lld\n", sortLength, current_timestamp());
            cnt = 0;
            for (i = 0; i < data_size; i = i + 4) {
                tmp =
                    (data_from_fpga[i + 3] << 24) | (data_from_fpga[i + 2] << 16) | (data_from_fpga[i + 1] << 8) |
                    data_from_fpga[i];
                array[cnt] = tmp;
                cnt++;
            }
	       finish = 1;
           /**
            for (i = 0; i < item_size; i++) {
                printf("inlib : %" PRIu32 "\n", array[i]);
            }**/
        }
    }

    printf("\n");

}

int mlog(double base, double x)
{
    return(int)(log(x) / log(base));
}

//------------------------------------------------------------------------
//MAIN: new thread to write, main thread to read
//------------------------------------------------------------------------
JNIEXPORT jintArray JNICALL Java_java_util_Arrays_dsort
    (JNIEnv *env, jobject obj, jintArray arr){
    int *arrIn = NULL;
    int i, counter;
    printf("in main: %lld\n", current_timestamp());

    jsize arrLength = (*env)->GetArrayLength(env, arr);
    jintArray arrSorted = (*env)->NewIntArray(env, arrLength);
    arrIn = (*env)->GetIntArrayElements(env, arr, 0);

    data_size = 4 * arrLength;
    data_to_fpga = (unsigned char*)calloc(data_size, sizeof(unsigned char));

    fprintf(stderr, "%d, %d\n", data_size, arrLength);

    fpga_write_fd = open("/dev/xillybus_write_32", O_WRONLY);

    if (fpga_write_fd < 0) {
        if (errno == ENODEV)
            fprintf(stderr, "(Maybe xillybus_write_32 a read-only file?)\n");

        perror("Failed to open devfile");
    }

    sortLength = 1;
    printf("in main: %lld\n", current_timestamp());
    loopsize = LOOPSIZE;

    counter = 0;
    for(i = 0; i < arrLength * 4; i += 4){
      //printf("%"PRIu32"\n",arrIn[counter]);
      data_to_fpga[i  ] |= (char)( arrIn[counter] & 0x000000FF);
      data_to_fpga[i+1] |= (char)((arrIn[counter] & 0x0000FF00) >> 8);
      data_to_fpga[i+2] |= (char)((arrIn[counter] & 0x00FF0000) >> 16);
      data_to_fpga[i+3] |= (char)((arrIn[counter] & 0xFF000000) >> 24);
      counter++;
    }

    item_size = data_size / 4;
    printf("data_size: %d, number of round: %d\n", data_size, mlog(loopsize, item_size));
    mround = mlog(loopsize, item_size);

    data_from_fpga = (unsigned char *)calloc(data_size, sizeof(unsigned char));
    array = (int32_t *)calloc(arrLength, sizeof(int32_t));

    fprintf(stderr, "data size: %d\n", data_size);

    pthread_t write_thread;
    if (pthread_create(&write_thread, NULL, writeStream, NULL)) {
        fprintf(stderr, "Error creating thread\n");
    }
    finish = 0;

    readStream();

    while(finish == 0){}

    printf("end main: %lld\n", current_timestamp());

    (*env)->SetIntArrayRegion(env, arrSorted, 0, arrLength, array);
    (*env)->ReleaseIntArrayElements(env, arr, arrIn, 0);
    free(array);
    free(data_to_fpga);
    free(data_from_fpga);
    close(fpga_write_fd);

    return arrSorted;
}

//------------------------------------------------------------------------
