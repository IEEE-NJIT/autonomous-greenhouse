#ifndef _UTIL_H
#define _UTIL_H

typedef uint8_t byte_t;
typedef int32_t bool32_t;
#define true    1
#define false   0

#ifdef DEBUG
#define ASSERT(x) if(!(x)) {*(volatile int*)0 = 0;}
#else 
#define ASSERT(x) 
#endif

#define AASSERT(x) if(!(x)) {*(volatile int*)0 = 0;} // Always assert even when not debugging

#define Kilobyte(x) (x * 1024LL)
#define Megabyte(x) (Kilobyte(x) * 1024LL)
#define Gigabyte(x) (Megabyte(x) * 1024LL)

// Return the length of a null-terminated array of arrays
size_t arrayLen( void *array[] ) {
    size_t i;
    for( i = 0; array[i] != NULL; ++i ) {}
    return i;
}

typedef struct {
    void *data;
    size_t size;
} FileBuffer;

FileBuffer openFile( const char *filename ) {
    FILE *fp = NULL;
    if( (fp = fopen( filename, "r" )) == NULL ) {
        fprintf( stderr, "File [%s] does not exist\n", filename );
        exit( -4 );
    }
    
    // Get file size
    size_t file_size = 0;
    fseek( fp, 0L, SEEK_END );
    file_size = ftell( fp ) + 1;
    fseek( fp, 0L, SEEK_SET );
    
    if( file_size == 0 ) {
        fprintf( stderr, "File [%s] could not be read\n", filename );
        exit( -2 );
    }

    FileBuffer buffer = {0};
    buffer.size = file_size-1;
    buffer.data = calloc( buffer.size, sizeof(char) );

    size_t bytes_read = fread( buffer.data, 1, file_size-1, fp );
    
    if( bytes_read == 0 || bytes_read != file_size-1 ) {
        fprintf( stderr, "Could not read file [%s]\n", filename );
        exit( -2 );
    }
    
    fclose( fp );

    return buffer;
}

bool32_t bufferFile( FileBuffer *buffer, const char *filename ) {
    FILE *fp = NULL;
    if( (fp = fopen( filename, "rb" )) == NULL ) {
        fprintf( stderr, "File [%s] does not exist\n", filename );
        return false;
    }
    
    // Get file size
    size_t file_size = 0;
    fseek( fp, 0L, SEEK_END );
    buffer->size = ftell( fp );
    fseek( fp, 0L, SEEK_SET );
    
    if( buffer->size == 0 ) {
        fprintf( stderr, "File [%s] could not be read\n", filename );
        fclose( fp );
        return false;
    }

    buffer->data = calloc( buffer->size+1, sizeof(char) );
    if( !buffer->data ) {
        fprintf( stderr, "Out of Memory: Allocation failed\n" );
        fclose( fp );
        return false;
    }

    fread( buffer->data, 1, buffer->size, fp );
    
    fclose( fp );

    return true;
}

void freeFileBuffer( FileBuffer *buffer ) {
    free( buffer->data );
    buffer->size = 0;
}

typedef struct {
    struct timeval begin;
    struct timeval end;
} TimeInterval;

void startTimeInterval(TimeInterval *time) {
    time->end.tv_sec = 0;
    time->end.tv_usec = 0;
    gettimeofday(&time->begin, 0);
}

void endTimeInterval(TimeInterval *time) {
    gettimeofday(&time->end, 0);
}

float calcTimeDelta(TimeInterval time) {
    long seconds = time.end.tv_sec - time.begin.tv_sec;
    long microseconds = time.end.tv_usec - time.begin.tv_usec;
    return (seconds + microseconds*1e-6);
}

#endif /* _UTIL_H */