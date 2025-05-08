
/* Simple V4L2 capture program for OX05B1S on i.MX8 */
#include <fcntl.h>
#include <stdio.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(void) {
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 2592;
    fmt.fmt.pix.height = 1944;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SRGGB12;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) { perror("S_FMT"); return 1; }

    printf("Format set, streaming check with v4l2-ctl.
");
    close(fd);
    return 0;
}
