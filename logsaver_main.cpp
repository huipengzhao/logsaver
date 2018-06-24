#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>
#include <sys/limits.h>
#include <sys/poll.h>
#include <linux/input.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>

#include <utils/SystemClock.h>

#define KLOGD_TIMEOUT_MS    (20*1000)
#define KLOGD_MAX_FILESIZE  (10*1024*1024) // in byte
#define KLOGD_BUF_BYTES     (8*1024)

int klogdump(char *kmsg, char *logfile) {
    int ret = 0;
    int total_wr_bytes = 0;
    int fd_in = 0, fd_out = 0;
    char *buf = (char*)malloc(KLOGD_BUF_BYTES);
    int64_t tm_start = android::uptimeMillis();

    errno = 0; // Clear errno.

    printf("Read from: %s\n", kmsg);
    printf("Write to : %s\n", logfile);
    printf("Max-size : %d B\n", KLOGD_MAX_FILESIZE);
    printf("Timeout  : %d ms\n", KLOGD_TIMEOUT_MS);

    if (buf == NULL) {
        fprintf(stderr, "Error, malloc(%d) failed, quit.\n", KLOGD_BUF_BYTES);
        ret = -1;
        goto out;
    }

    if (kmsg == NULL || logfile == NULL) {
        fprintf(stderr, "Error, kmsg or logfile must not be NULL, quit.\n");
        ret = -1;
        goto out;
    }

    // Open kmsg, typically it is /dev/kmsg.
    fd_in = open(kmsg, O_RDONLY|O_NONBLOCK);
    if (fd_in < 0) {
        // error
        fprintf(stderr, "Failed to open %s, error: %s\n", kmsg, strerror(errno));
        ret = -errno;
        goto out;
    }

    // Open logfile, like /data/kmsg.
    // Make sure the directory of the log file path exists.
    fd_out = open(logfile, O_WRONLY|O_CREAT, 0660);
    if (fd_out < 0) {
        //error
        fprintf(stderr, "Failed to open %s, error: %s\n", logfile, strerror(errno));
        ret = -errno;
        goto out;
    }

    // Read from kmsg, and write to logfile.
    fd_set rfds;
    struct timeval tv;

    // Watch fd_in to see when it has input.
    // Wait up to 2 seconds.
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(fd_in, &rfds);

        ret = select(fd_in+1, &rfds, NULL, NULL, &tv);
        // Don't rely on the value of tv now!

        if (ret == -1) {
            // Error happened.
            fprintf(stderr, "select() failed, error: %s\n", strerror(errno));
        } else
        if (ret) {
            // Data available.
            if (FD_ISSET(fd_in, &rfds)) {
                int nr, nw;
                while (1) {
                    // Read from kmsg.
                    nr = read(fd_in, buf, KLOGD_BUF_BYTES);
                    if (nr <= 0) break; // No more to read.

                    // Write to log file.
                    nw = write(fd_out, buf, nr);
                    if (nw <= 0) {
                        fprintf(stderr, "write(%d) returns %d\n", nr, nw);
                        // Maybe should stop.
                    } else {
                        total_wr_bytes += nw;
                    }
                }
            }
        } else {
            // Just timeout.
        }

        if (android::uptimeMillis() - tm_start > KLOGD_TIMEOUT_MS) {
            printf("Reach timeout(%d ms), stop.\n", KLOGD_TIMEOUT_MS);
            break;
        }

        if (total_wr_bytes >= KLOGD_MAX_FILESIZE) {
            printf("Reach max file size(%d bytes), stop.\n", KLOGD_MAX_FILESIZE);
            break;
        }
    } // while(1)

out:
    if (fd_in)  close(fd_in);
    if (fd_out) close(fd_out);
    if (buf)    free(buf);
    return ret;
}

void show_usage() {
    printf("Usage:\n");
    printf("    logsaver -k <kmsg> <logfile>\n");
}

int main(int argc, char *argv[]) {
    bool dump_k = false;
    bool dump_a = false;

    if (argc != 4) {
        show_usage();
        return -1;
    }

    if (!strcmp(argv[1], "-k")) {
        dump_k = true;
    } else
    if (!strcmp(argv[1], "-a")) {
        dump_a = true;
    } else {
        show_usage();
        return -1;
    }

    if (dump_k) {
        klogdump(argv[2], argv[3]);
    }

    return 0;
}
