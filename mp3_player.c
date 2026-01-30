#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mpg123.h>
#include <alsa/asoundlib.h>

#define BUFFER_SIZE 32768

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mp3_file>\n", argv[0]);
        exit(1);
    }

    const char *filename = argv[1];
    
    // Initialize mpg123
    int err;
    mpg123_handle *mh = NULL;
    
    err = mpg123_init();
    if (err != MPG123_OK) {
        fprintf(stderr, "Error initializing mpg123: %s\n", mpg123_plain_strerror(err));
        exit(1);
    }
    
    mh = mpg123_new(NULL, &err);
    if (mh == NULL) {
        fprintf(stderr, "Error creating mpg123 handle: %s\n", mpg123_plain_strerror(err));
        mpg123_exit();
        exit(1);
    }
    
    // Open the MP3 file
    err = mpg123_open(mh, filename);
    if (err != MPG123_OK) {
        fprintf(stderr, "Error opening MP3 file: %s\n", mpg123_strerror(mh));
        mpg123_delete(mh);
        mpg123_exit();
        exit(1);
    }
    
    // Get audio format
    long rate;
    int channels, encoding;
    err = mpg123_getformat(mh, &rate, &channels, &encoding);
    if (err != MPG123_OK) {
        fprintf(stderr, "Error getting format: %s\n", mpg123_strerror(mh));
        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();
        exit(1);
    }
    
    // Convert mpg123 encoding to ALSA format
    snd_pcm_format_t pcm_format;
    switch (encoding) {
        case MPG123_ENC_SIGNED_16:
            pcm_format = SND_PCM_FORMAT_S16_LE;
            break;
        case MPG123_ENC_FLOAT_32:
            pcm_format = SND_PCM_FORMAT_FLOAT_LE;
            break;
        default:
            fprintf(stderr, "Unsupported encoding\n");
            mpg123_close(mh);
            mpg123_delete(mh);
            mpg123_exit();
            exit(1);
    }
    
    // Open ALSA device
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    int dir;
    
    if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Cannot open audio device\n");
        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();
        exit(1);
    }
    
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, pcm_format);
    snd_pcm_hw_params_set_channels(handle, params, channels);
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, 0);
    snd_pcm_hw_params(handle, params);
    
    printf("Playing MP3 file: %s\n", filename);
    printf("Sample Rate: %ld Hz\n", rate);
    printf("Channels: %d\n", channels);
    printf("Encoding: %d\n", encoding);
    
    // Allocate buffer for audio data
    unsigned char buffer[BUFFER_SIZE];
    size_t done;
    
    // Play the audio
    while (mpg123_read(mh, buffer, BUFFER_SIZE, &done) == MPG123_OK && done > 0) {
        snd_pcm_sframes_t frames = snd_pcm_writei(handle, buffer, done / (channels * sizeof(short)));
        
        if (frames < 0) {
            frames = snd_pcm_recover(handle, frames, 0);
            if (frames < 0) {
                fprintf(stderr, "Write error: %s\n", snd_strerror(frames));
                break;
            }
        }
    }
    
    // Drain and close
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    
    // Cleanup
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    
    printf("\nFinished playing: %s\n", filename);
    return 0;
}