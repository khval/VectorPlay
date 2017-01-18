
#include <proto/exec.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <AL/al.h>
#include <AL/alut.h>
#include <vorbis/vorbisfile.h>
#include <cstdio>
#include <iostream>
#include <vector>

#include <mpg123.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_sound.h>

#define swap32( x ) ((( x & 0xFF )<<24) | (( x & 0xFF00 )<<8) | (( x &0xFF0000 )>>8) | (( x &0xFF000000 )>>24))

#define swap16( x ) ((( x & 0xFF )<<8) | (( x & 0xFF00 )>>8)) 

#define chkid( a, b ) ((a[0]==b[0])&&(a[1]==b[1])&&(a[2]==b[2])&&(a[3]==b[3]))

struct wav_chunk{
	char id[4];
	int 	size;
};

struct wav_fmt
{
	uint16 compression;
	uint16 channels;
	uint32 samplerate;
	uint32 byterate;
	uint16 blockallign;
	uint16 bits;
};


static void swap16mem( uint16_t *mem, int size );
static void read_chunk(struct wav_chunk *chunk,FILE *file);
static int read_fmt(struct wav_fmt *fmt,FILE *file);
static inline ALenum GetFormatFromInfo(uint16_t channels, uint16_t bits);

// maybe need to mutex protect?
static int _allegro_num_voices = 0;
static int _allegro_voice_loop = 0;

ALLEGRO_MIXER mixer_reserved[20];

static ALuint  _allegro_voices[10];


ALLEGRO_MIXER *al_get_default_mixer()
{
	int n = 0;
	for (n = 0; n<_allegro_num_voices;n++)
	{
		printf("%d, %d, %08x\n",n,mixer_reserved[n].voice,mixer_reserved[n].instance);

		if (mixer_reserved[n].instance == NULL )
		{
			printf("al_get_default_mixer = %d\n",n);
			mixer_reserved[n].voice = n;
			return &mixer_reserved[n];
		}

	}
	return NULL;	
}

void al_reserve_samples(int samples)
{
	int n;
	if (samples>4) samples = 4;

	printf("al_reserve_samples(int samples)\n");

	_allegro_num_voices = samples;

	for (n = 0; n<_allegro_num_voices;n++)
	{
		mixer_reserved[n].instance = NULL;
	}

	printf("al_reserve_samples(%d)\n",samples);

	for (int n=0;n<samples;n++)
	{
		printf("%d\n",n);
		alGenSources (n+1, &_allegro_voices[n]);
	}
}

void al_play_sample( ALLEGRO_SAMPLE *sound, double gain , double pan , double speed,  int mode, void *ptr )
{
	do
	{
		_allegro_voice_loop=(_allegro_voice_loop+1) % _allegro_num_voices;
	} while (mixer_reserved[ _allegro_voice_loop ].instance);

//	alSourcei( _allegro_voices[ instance -> voice ], AL_LOOPING, instance -> flags == ALLEGRO_PLAYMODE_LOOP );
	alSourcei(_allegro_voices[ _allegro_voice_loop ], AL_BUFFER, sound -> ALbuffer );
	alSourcePlay (_allegro_voices[ _allegro_voice_loop ]);
}


BOOL al_set_sample_instance_playmode( ALLEGRO_SAMPLE_INSTANCE *instance, ALLEGRO_PLAYMODE val)
{
	instance -> playmode = val;
	return TRUE;
}

BOOL al_play_sample_instance( ALLEGRO_SAMPLE_INSTANCE *instance )
{
	printf(" instance -> voice %d\n", instance -> voice);

	if ( instance -> voice > -1)
	{
		printf("play song\n");

		alSourcei( _allegro_voices[ instance -> voice ], AL_LOOPING, instance -> playmode == ALLEGRO_PLAYMODE_LOOP );
		alSourcei( _allegro_voices[ instance -> voice ], AL_BUFFER, instance -> sample -> ALbuffer );

		alSourcePlay (_allegro_voices[ instance -> voice ]);
	}

}

BOOL al_attach_sample_instance_to_mixer( ALLEGRO_SAMPLE_INSTANCE *instance, ALLEGRO_MIXER *mixer)
{
	if (mixer)
	{
		printf("seting mixer\n");
		mixer -> instance = instance;
		printf("setting instance\n");
		instance -> voice = mixer -> voice;
		return TRUE;
	}
	return FALSE;
}

BOOL al_detach_sample_instance_to_mixer( ALLEGRO_SAMPLE_INSTANCE *instance)
{
	if (instance -> voice>-1)
	{
		mixer_reserved[instance -> voice].instance = NULL;
		instance -> voice = -1;
	}
}

void al_destroy_sample( ALLEGRO_SAMPLE *sample )
{
	if (sample)
	{
		free(sample->data);
		free(sample);
	}
}

ALfloat listenerPos[]={0.0,0.0,4.0};
ALfloat listenerVel[]={0.0,0.0,0.0};
ALfloat listenerOri[]={0.0,0.0,0.0};


void al_install_audio()
{
	printf("al_install_audio()\n");

	_allegro_num_voices = 0;


	alListenerfv(AL_POSITION,listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION,listenerOri);
	alGetError();
}



ALLEGRO_SAMPLE_INSTANCE *al_create_sample_instance( ALLEGRO_SAMPLE *sample )
{
	ALLEGRO_SAMPLE_INSTANCE *instance;

	if (instance = (ALLEGRO_SAMPLE_INSTANCE *) malloc(sizeof(ALLEGRO_SAMPLE_INSTANCE)))
	{
		instance -> sample = sample;
		instance -> playmode = 0;
		instance -> voice = -1;
	}
	return instance;
}

void al_destroy_sample_instance( ALLEGRO_SAMPLE_INSTANCE *instance )
{
	free ( instance );
}

ALLEGRO_SAMPLE *al_load_sample_wav( char *name);
ALLEGRO_SAMPLE *al_load_sample_ogg( char *name);
ALLEGRO_SAMPLE *al_load_sample_mp3( char *name);

ALLEGRO_SAMPLE *al_load_sample( char *name)
{
	char *ext[]={(char *) ".wav",(char *) ".ogg",(char *) ".mp3", NULL};
	int n;
	ALLEGRO_SAMPLE *sample = NULL;

	for (n =0; ext[n]; n++ )
	{
		if (strlen(name)>strlen(ext[n]))
		{
			if (strcasecmp(name +strlen(name) - strlen(ext[n]),ext[n])==0)
			{
				switch ( n )
				{
					case 0: sample = al_load_sample_wav( name ); break;
					case 1: sample = al_load_sample_ogg( name ); break;
					case 2: sample = al_load_sample_mp3( name); break;
				}
				break;
			}
		}
	}

	if (sample)
	{
		sample -> duration  = ((float) sample ->size) / sample -> byterate; 

		alGenBuffers( 1, &sample -> ALbuffer );
		alBufferData(  sample -> ALbuffer , sample -> format, sample -> data, sample -> size, (float) sample -> samplerate );
	}

	return sample;
}

ALLEGRO_SAMPLE *al_load_sample_mp3( char *name)
{
	ALLEGRO_SAMPLE *sound = NULL;

	unsigned char *newbuffer;

	mpg123_handle *mh;
	unsigned char *buffer;
	size_t buffer_size;
	size_t done;
	int err;
	int channels, encoding;
	long rate;
	unsigned int newsize;

	mpg123_init();
	mh = mpg123_new(NULL, &err);
	buffer_size = mpg123_outblock(mh);

	sound = (ALLEGRO_SAMPLE *) malloc(sizeof(ALLEGRO_SAMPLE));

	if ((sound)&&(mh))
	{
		memset(sound,0,sizeof(ALLEGRO_SAMPLE));

		buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

		if (buffer)
		{
			/* open the file and get the decoding format */
			mpg123_open(mh, name);
			mpg123_getformat(mh, &rate, &channels, &encoding);

			sound -> bits = mpg123_encsize(encoding) * 8;
			sound -> channels = channels;
			sound -> byterate = rate;
			sound -> samplerate = rate;
			sound -> format = GetFormatFromInfo(channels, sound -> bits);

			sound -> size = 0;
			sound -> data = NULL;

			newsize = 0;

			/* decode */
			while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
			{
				newsize += done;
	
				if (newbuffer = (unsigned char *) malloc(newsize))
				{
					if (sound -> data)
					{
						memcpy(newbuffer,sound ->data, sound->size);	// old decoded mp3
						free (sound -> data);
					}
					memcpy(newbuffer + sound->size, buffer, done);		// new decoded mp3
					sound -> data = newbuffer;
					sound -> size = newsize;
				}
				else
				{
					printf("sorry can't get more music\n");
					newsize = sound -> size;
				}


				if (newsize>11000000) 
				{
					printf("song is too long\n");
					break;
				}
			}
		}
	}

	if (buffer)
	{
		free (buffer);
		buffer = NULL;
	}

	if (mh)
	{
		mpg123_delete(mh);
		mh = NULL;
	}

	mpg123_exit();

	return sound;
}

ALLEGRO_SAMPLE *al_load_sample_ogg( char *name)
{
	ALLEGRO_SAMPLE *sound = NULL;
	FILE *file;
	vorbis_info *info;
	OggVorbis_File	ogg_file;
	int section = 0;
	int size = 0,result;

	if (file = fopen( name ,"rb"))
	{
		if (sound = (ALLEGRO_SAMPLE *) malloc(sizeof(ALLEGRO_SAMPLE)))
		{
			memset(sound,0,sizeof(ALLEGRO_SAMPLE));
	
			rewind(file);

			if (ov_open(file,&ogg_file, NULL, 0) < 0)
			{
				fclose(file);	
			}

			info = ov_info(&ogg_file, -1);

			sound -> bits = 16;
			sound -> channels = info -> channels;
			sound -> samplerate = info -> rate;
			sound -> format = info -> channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

			sound -> size = ov_pcm_total( &ogg_file, -1 ) * info -> channels *2;
			sound -> data = (unsigned char *) malloc( sound -> size);

			while (size < sound -> size)
			{
				result = ov_read(&ogg_file, (char *) sound -> data + size, sound -> size, 0, 2, 1, &section );
				if (result > 0)
				{
					size += result;
				}
				else if (result < 0)
				{
					printf("---- failed to load\n");

					ov_clear( &ogg_file );
					free (sound -> data);
					free (sound);
					return NULL;
				}
				else
				{
					break;
				}
			}

			swap16mem( (uint16_t *) sound ->data, sound -> size);
		}
		ov_clear( &ogg_file);
	}

	printf("sound at %08x\n",sound);

	return sound;
}



ALLEGRO_SAMPLE *al_load_sample_wav( char *name)
{
	ALLEGRO_SAMPLE *sound;
	FILE* file;
	struct wav_chunk chunk;
	struct wav_chunk subchunk;
	struct wav_fmt fmt;
	char format[5];
	ALuint buffer;
	int vaild_format = 0;
	int cread;

	file = fopen(name, "rb");
	if (!file)
	{
		printf("file not open\n");
		return NULL;
	}

	if (sound = (ALLEGRO_SAMPLE *) malloc(sizeof(ALLEGRO_SAMPLE)))
	{
		memset(sound,0,sizeof(ALLEGRO_SAMPLE));
	}
	else
	{
		printf("sound struct not allocated\n");
		fclose(file);
		return NULL;
	}

	read_chunk( &chunk, file );

	printf("%c%c%c%c\n", chunk.id[0],chunk.id[1],chunk.id[2],chunk.id[3]);
	printf("chunk size %d\n",chunk.size);

	fread( (char*) format, 4, 1, file );
	format[4]=0;

	if (strcmp(format,"WAVE")!=0) 
	{
		printf("Not wave file\n");
		free(sound);
		fclose(file);
		return NULL;
	}
	
	subchunk.id[0] = '!';
	cread = 0;
	subchunk.size = 0;
	while ( subchunk.id[0] )
	{

		fseek( file, subchunk.size - cread , SEEK_CUR  );
		read_chunk( &subchunk, file );
		cread = 0;

		printf("%c%c%c%c - %d\n", subchunk.id[0],subchunk.id[1],subchunk.id[2],subchunk.id[3], subchunk.size);

		if (chkid( subchunk.id, "fmt "))
		{
			cread = read_fmt( &fmt, file);
			sound -> format = GetFormatFromInfo(fmt.channels, fmt.bits);
			sound -> bits = fmt.bits;
			sound -> channels = fmt.channels;
			sound -> byterate = fmt.byterate;
			sound -> samplerate = fmt.samplerate;
			
			printf("compression %d\n", fmt.compression);

		} else if (chkid( subchunk.id, "data"))
		{
			sound -> data = (unsigned char *) malloc( subchunk.size );
			fread(sound -> data, subchunk.size,1, file);

			if (sound -> bits == 16)
			{
				swap16mem( (uint16_t *) sound ->data, subchunk.size);
			}

			sound -> size = subchunk.size;
		}
	}

	fclose(file);
	return sound;
}

static int read_fmt(struct wav_fmt *fmt,FILE *file)
{
	int n = fread( fmt, sizeof(struct wav_fmt), 1, file);
	if (!n) memset( fmt, 0, sizeof(struct wav_fmt) );

	fmt -> compression = swap16(fmt -> compression);
	fmt -> channels = swap16(fmt -> channels);
	fmt -> samplerate = swap32(fmt -> samplerate);
	fmt -> byterate = swap32(fmt -> byterate);
	fmt -> blockallign = swap16(fmt -> blockallign);
	fmt -> bits = swap16(fmt -> bits);

	return n * sizeof(struct wav_fmt);
}

static void swap16mem( uint16_t *mem, int size )
{
	int n;
	size /= 2;
	for (n=0;n<size;n++)	mem[n] = swap16( mem[n] );
}

static void read_chunk(struct wav_chunk *chunk,FILE *file)
{
	int n = fread( chunk, sizeof(struct wav_chunk), 1, file);
	if (!n) memset( chunk, 0, sizeof(struct wav_chunk) );
	chunk -> size = swap32( chunk -> size );
}


static inline ALenum GetFormatFromInfo(uint16_t channels, uint16_t bits)
{
	uint32 format = 0;

	if (channels == 1)
	{
		switch (bits)
		{
			case 8:	format = AL_FORMAT_MONO8; 	break;
			case 16:	format = AL_FORMAT_MONO16; 	break;
		}
	}
	else
	{
		switch (bits)
		{
			case 8:	format = AL_FORMAT_STEREO8; 	break;
			case 16:	format = AL_FORMAT_STEREO16; 	break;
		}
	}

	printf("audio format: %d\n",format);

	return format;
}


