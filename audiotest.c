#include <portaudio.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <stdint.h>

#define FS (48*1000)
#define BUFTIME 1

int32_t strbuf_front[FS*BUFTIME*2];
int32_t strbuf_back[FS*BUFTIME*2];
int32_t audbuf[FS*BUFTIME]; // Signed int32

static uint32_t
xor128(void){ // From Wikipedia:Ja Xorshift
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123; 
    uint32_t t;

    t = x ^ (x << 11);
    x = y; y = z; z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)); 
}

static void
init_audbuf(void){
    int i;
    for(i=0;i!=FS*BUFTIME;i++){
        audbuf[i] = ((int32_t)xor128())/5;
    }

    /* Fill `front` waveform */
    memset(strbuf_front,0,sizeof(strbuf_front));
    for(i=0;i!=FS*BUFTIME*2;i+=2){
        /* Center */
        strbuf_front[i] = audbuf[i/2];
        strbuf_front[i+1] = audbuf[i/2];
    }

    /* Fill `back` waveform */
    memset(strbuf_back,0,sizeof(strbuf_back));
    for(i=0;i!=FS*BUFTIME*2;i+=2){
        /* Center */
        strbuf_back[i] = audbuf[i/2];
        strbuf_back[i+1] = -audbuf[i/2]; /* invert */
    }
}

static void
init_pa(void){
    PaError e;
    e = Pa_Initialize();
    if(e != paNoError){
        printf("PortAudio init error = %d\n",e);
        exit(-1);
    }
}


static void
run(void){
    PaError e;
    PaStream* str;
    int i;

    e = Pa_OpenDefaultStream(&str,
                             0,
                             2,
                             paInt32,
                             FS,
                             paFramesPerBufferUnspecified,
                             NULL /* Use Blocking mode */,
                             NULL);
    if(e != paNoError){
        printf("PortAudio open error = %d\n",e);
        exit(-1);
    }

    e = Pa_StartStream(str);
    if(e != paNoError){
        printf("PortAudio start error = %d\n",e);
        exit(-1);
    }

    /* Start output */
    for(;;){
        printf("Front.\n");
        e = Pa_WriteStream(str, strbuf_front, FS*BUFTIME);
        if(e != paNoError){
            goto endgame_err;
        }
        printf("Back.\n");
        e = Pa_WriteStream(str, strbuf_back, FS*BUFTIME);
        if(e != paNoError){
            goto endgame_err;
        }
    }
    return;
endgame_err:
    printf("PortAudio write error = %d\n",e);
    exit(-1);
}

int
main(int ac, char** av){
    init_audbuf();
    init_pa();
    run();
    return 0;
}
