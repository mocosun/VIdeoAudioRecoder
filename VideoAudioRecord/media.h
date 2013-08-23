#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif
using namespace std;
class media
{
public:
	media();
	~media();
	void setVideoFormat(int width, int height, int bit_rate=15000);
	void setAudioForamt(int bit_rate, int sample_rate);

	int openFile(string dstPath);
	void closeFile();
	void addPicture(unsigned char* rgb, int frameDiff = 1);
	void addAudio(unsigned char* data, int len);


private:
	AVStream *add_stream(AVFormatContext *oc, AVCodec **codec,enum AVCodecID codec_id);
	void open_audio( AVCodec *codec, AVStream *st);
	void open_video(AVCodec *codec, AVStream *st);
	void close_audio(AVStream *st);
	void close_video(AVStream *st);
	void write_audio_frame(AVFormatContext *oc, AVStream *st, unsigned char *data, int len);
	void write_video_frame(AVFormatContext *oc, AVStream *st, unsigned char *rgb);
	void get_audio_frame(int16_t *samples,unsigned char *data, int len);


	AVOutputFormat *fmt;
	AVFormatContext *oc;
	AVStream *audio_st, *video_st;
	AVCodec *audio_codec, *video_codec;
	double audio_pts, video_pts;

	int audio_input_frame_size;					//音频帧长度
	uint8_t *samples_buf;						//音频数据缓冲区
	int samples_buf_maxSize;                    //音频数据缓冲区最大容量
	int samples_buf_size;                       //音频数据缓冲区当前数据量

	AVFrame *frame;						 //视频帧（可重用）
	AVPicture src_picture, dst_picture;  //视频图像
	int frame_count;                     //帧计数，用于计算PTS
	int video_width, video_height;
	int video_bit_rate;
	int audio_bitrate, audio_sample_rate;

	HANDLE mutex;
};

media::media()
{
	video_st = NULL;
	audio_st = NULL;
	audio_bitrate = 64000;
	audio_sample_rate = 44100;
	video_width = 1280;
	video_height = 720;
	video_bit_rate = 15000;
	samples_buf_size = 0;
	av_register_all();

	mutex = CreateMutex (NULL, FALSE, NULL);

}

media::~media()
{
	avformat_free_context(oc);

	ReleaseMutex(mutex);
}

void media::setVideoFormat(int width, int height, int bit_rate)
{
	video_width = width;
	video_height = height;
	video_bit_rate = bit_rate;
}
void media::setAudioForamt(int bit_rate, int sample_rate)
{
	audio_bitrate = bit_rate;
	audio_sample_rate = sample_rate;
}
/*************************
//函数功能：打开输出文件
//输入：输出文件的路径
//输出：0-成功；1-AVFormatContext初始失败；2-无法写入路径
**************************/
int media::openFile(string dstPath)
{
	int ret;
	avformat_alloc_output_context2(&oc, NULL, NULL, dstPath.c_str());
	if (!oc) {
		printf("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&oc, NULL, "mpeg", dstPath.c_str());
	}
	if (!oc) {
		return 1;
	}
	fmt = oc->oformat;
	if (fmt->video_codec != AV_CODEC_ID_NONE) {
		video_st = add_stream(oc, &video_codec, fmt->video_codec);
	}
	if (fmt->audio_codec != AV_CODEC_ID_NONE) {
	audio_st = add_stream(oc, &audio_codec, fmt->audio_codec);
	}

	if (video_st)
		open_video(video_codec, video_st);
	if (audio_st)
		open_audio(audio_codec, audio_st);

	 av_dump_format(oc, 0, dstPath.c_str(), 1);

	 /* open the output file, if needed */
	 if (!(fmt->flags & AVFMT_NOFILE)) {
		 ret = avio_open(&oc->pb, dstPath.c_str(), AVIO_FLAG_WRITE);
		 if (ret < 0) {
			 fprintf(stderr, "Could not open '%s': %d\n", dstPath.c_str(),
				 ret);
			 return 2;
		 }
	 }

	 /* Write the stream header, if any. */
	 ret = avformat_write_header(oc, NULL);
	 if (ret < 0) {
		 fprintf(stderr, "Error occurred when opening output file: %d\n",
			 ret);
		 return 1;
	 }

	 if (frame)
		 frame->pts = av_rescale_q(1, video_st->codec->time_base, video_st->time_base); 
	return 0;
}

/*************************
//函数功能：关闭输出文件
//输入：输出文件的路径
//输出：0-成功；1-失败
**************************/
void media::closeFile()
{
	  /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(oc);

    /* Close each codec. */
    if (video_st)
        close_video(video_st);
    if (audio_st)
        close_audio(audio_st);

    if (!(fmt->flags & AVFMT_NOFILE))
       avio_close(oc->pb);   /* Close the output file. */

}

/*************************
//函数功能：增量写入一张图片
//输入：rgb--图片数据（注意数据大小应保持一致）； frameDiff——该图片的持续帧数，通常等于1.0，表示持续1/25秒
//输出：无
**************************/
void media::addPicture(unsigned char* rgb, int frameDiff)
{
	if (video_st)
		video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den * frameDiff;
	else
		video_pts = 0.0;

	WaitForSingleObject( mutex, INFINITE );
	write_video_frame(oc, video_st, rgb);
	ReleaseMutex(mutex);
	
	if(frameDiff < 25)
		frame->pts += av_rescale_q(1, video_st->codec->time_base, video_st->time_base);    //1* 1/25 * 9000 
	else
		frame->pts += av_rescale_q(1, video_st->codec->time_base, video_st->time_base)* av_rescale(1, frameDiff, 25);    //1* 1/25 * 9000 * frameDiff
}
/*************************
//函数功能：增量写入一个音频帧
//输入：data--音频数据（注意数据大小应保持一致）；len--长度
//输出：无
**************************/

void media::addAudio(unsigned char* data, int len)
{
	if (audio_st)
		audio_pts = (double)audio_st->pts.val * audio_st->time_base.num / audio_st->time_base.den;
	else
		audio_pts = 0.0;

	WaitForSingleObject( mutex, INFINITE );
	write_audio_frame(oc, audio_st, data, len);
	ReleaseMutex(mutex);
}

#define STREAM_FRAME_RATE 25
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P  
/* Add an output stream. */
AVStream *media::add_stream(AVFormatContext *oc, AVCodec **codec,
                            enum AVCodecID codec_id)
{
    AVCodecContext *c;
    AVStream *st;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }

    st = avformat_new_stream(oc, *codec);
    if (!st) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }
    st->id = oc->nb_streams-1;
    c = st->codec;

    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        st->id = 1;
        c->sample_fmt  = AV_SAMPLE_FMT_S16;
        c->bit_rate    = audio_bitrate;
        c->sample_rate = audio_sample_rate;
        c->channels    = 1;
        break;

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;

        c->bit_rate = video_bit_rate;;
        /* Resolution must be a multiple of two. */
        c->width    = video_width;
        c->height   = video_height;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        c->time_base.den = STREAM_FRAME_RATE;
        c->time_base.num = 1;
        c->gop_size      = 3; /* emit one intra frame every twelve frames at most */
        c->pix_fmt       = STREAM_PIX_FMT;
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B frames */
            c->max_b_frames = 2;
        }
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
            /* Needed to avoid using macroblocks in which some coeffs overflow.
             * This does not happen with normal video, it just happens here as
             * the motion of the chroma plane does not match the luma plane. */
            c->mb_decision = 2;
        }
    break;

    default:
        break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return st;
}


void media::open_audio( AVCodec *codec, AVStream *st)
{
	AVCodecContext *c = st->codec;
	int ret;

	/* open it */
	ret = avcodec_open2(c, codec, NULL);
	if (ret < 0) {
		fprintf(stderr, "Could not open audio codec: %d\n", ret);
		exit(1);
	}

	if (c->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE)
		audio_input_frame_size = 10000;
	else
		audio_input_frame_size = c->frame_size;
	samples_buf = (uint8_t *)av_malloc(audio_input_frame_size *
		av_get_bytes_per_sample(c->sample_fmt) *
		c->channels * 10);  //10倍编码帧长的缓冲区
	samples_buf_maxSize = audio_input_frame_size * av_get_bytes_per_sample(c->sample_fmt) *	c->channels*10;

	if (!samples_buf) {
		fprintf(stderr, "Could not allocate audio samples buffer\n");
		exit(1);
	}
}

void media::open_video(AVCodec *codec, AVStream *st)
{
    int ret;
    AVCodecContext *c = st->codec;

    /* open the codec */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec: %d\n", ret);
        exit(1);
    }

    /* allocate and init a re-usable frame */
    frame = avcodec_alloc_frame();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    /* Allocate the encoded raw picture. */
    ret = avpicture_alloc(&dst_picture, c->pix_fmt, c->width, c->height);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate picture: %d\n", ret);
        exit(1);
    }

    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
   
        ret = avpicture_alloc(&src_picture, AV_PIX_FMT_BGR24, c->width, c->height);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate temporary picture: %d\n",
                    ret);
            exit(1);
        }
    

    /* copy data and linesize picture pointers to frame */
    *((AVPicture *)frame) = dst_picture;
}



void media::close_audio(AVStream *st)
{
	AVPacket pkt;
	int got_output;
	for (got_output = 1; got_output; ) {
		int ret = avcodec_encode_audio2(st->codec, &pkt, NULL, &got_output);
		if (ret < 0) {
			fprintf(stderr, "Error encoding frame\n");
			exit(1);
		}

		pkt.stream_index = st->index;

		if (got_output) {
			ret = av_interleaved_write_frame(oc, &pkt);
			av_free_packet(&pkt);
			if (ret != 0) {
				fprintf(stderr, "Error while writing audio frame: %d\n",
					ret);
				exit(1);
			}
		}
	}
	avcodec_close(st->codec);
	//av_free(samples);
}

void media::close_video(AVStream *st)
{
	avcodec_close(st->codec);
	//av_free(src_picture.data[0]);  //读取的是外部的数据，不能删除
	av_free(dst_picture.data[0]);
	av_free(frame);
}

void media::get_audio_frame(int16_t *samples,unsigned char *data, int len)
{
	int16_t *p = samples;
	int samplesLen = len/2;
	for(int i=0; i<samplesLen; i++)
	{
		short b = 0;
		short t = (int)(data[i*2]);
		short tg =(short)(data[i*2+1]);
		tg = tg << 8;
		b = tg | t;
		*p++ = t;
	}
}

void media::write_audio_frame(AVFormatContext *oc, AVStream *st, unsigned char *data, int len)
{
	if(len + samples_buf_size < samples_buf_maxSize)
	{
		memcpy(samples_buf+samples_buf_size, data, len);
		samples_buf_size += len;
	}
	else
	{
		//缓冲区将满，将其填满，编码后，再把剩余字节填充进去
		int nbToFull = samples_buf_maxSize - samples_buf_size;
		memcpy(samples_buf+samples_buf_size, data, nbToFull);

		//编码
		AVCodecContext *c = st->codec;

		int got_packet, ret;
		for (int i=0; i<10; i++)
		{
			AVPacket pkt = { 0 }; // data and size must be 0;
			AVFrame *frame = avcodec_alloc_frame();
			av_init_packet(&pkt);
			//get_audio_frame(samples, data, len);
			frame->nb_samples = audio_input_frame_size;
			avcodec_fill_audio_frame(frame, c->channels, c->sample_fmt,
				(uint8_t *)samples_buf + i * audio_input_frame_size * av_get_bytes_per_sample(c->sample_fmt),
				audio_input_frame_size *
				av_get_bytes_per_sample(c->sample_fmt) *
				c->channels, 0);

			ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
			if (ret < 0) {
				fprintf(stderr, "Error encoding audio frame: %d\n", ret);
				exit(1);
			}

			if (!got_packet)
				return;

			pkt.stream_index = st->index;

			/* Write the compressed frame to the media file. */
			ret = av_interleaved_write_frame(oc, &pkt);
			if (ret != 0) {
				fprintf(stderr, "Error while writing audio frame: %d\n",
					ret);
				exit(1);
			}
			avcodec_free_frame(&frame);
		}
	
		//填充剩余字节
		int nbLeft = len - nbToFull;
		memcpy(samples_buf, data+nbToFull, nbLeft);
		samples_buf_size = 0;
	}

	
}

void media::write_video_frame(AVFormatContext *oc, AVStream *st, unsigned char* rgb)
{
    int ret;
    static struct SwsContext *sws_ctx;
    AVCodecContext *c = st->codec;


            /* as we only generate a YUV420P picture, we must convert it
             * to the codec pixel format if needed */
            if (!sws_ctx) {
                sws_ctx = sws_getContext(c->width, c->height, AV_PIX_FMT_BGR24,
                                         c->width, c->height, AV_PIX_FMT_YUV420P,
                                         SWS_BICUBIC, NULL, NULL, NULL);
                if (!sws_ctx) {
                    fprintf(stderr,
                            "Could not initialize the conversion context\n");
                    return;
                }
            }

			/*	unsigned char* p_rgbData = rgb;
			int x,y;
			for (y = 0; y < c->height; y++) {
			for (x = 0; x < c->width; x++) {
			src_picture.data[0][y * src_picture.linesize[0] + x] = *p_rgbData++;
			src_picture.data[1][y * src_picture.linesize[1] + x] = *p_rgbData++;
			src_picture.data[2][y * src_picture.linesize[2] + x] = *p_rgbData++;
			}
			}*/
			src_picture.data[0] = (uint8_t*)rgb;
			//src_picture.data[1] = (uint8_t*)rgb+c->width*c->height;
			//src_picture.data[2] = (uint8_t*)rgb+c->width*c->height*2;
			
            sws_scale(sws_ctx,
                      (const uint8_t * const *)src_picture.data, src_picture.linesize,
                      0, c->height, dst_picture.data, dst_picture.linesize);
    
    

    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* Raw video case - directly store the picture in the packet */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags        |= AV_PKT_FLAG_KEY;
        pkt.stream_index  = st->index;
        pkt.data          = dst_picture.data[0];
        pkt.size          = sizeof(AVPicture);

        ret = av_interleaved_write_frame(oc, &pkt);
    } else {
        AVPacket pkt = { 0 };
        int got_packet;
        av_init_packet(&pkt);

        /* encode the image */
        ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
        if (ret < 0) {
            fprintf(stderr, "Error encoding video frame: %d\n",ret);
            //exit(1);
			return;
        }
        /* If size is zero, it means the image was buffered. */

        if (!ret && got_packet && pkt.size) {
            pkt.stream_index = st->index;

            /* Write the compressed frame to the media file. */
            ret = av_interleaved_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
        fprintf(stderr, "Error while writing video frame: %d\n", ret);
        exit(1);
    }
    frame_count++;
}


