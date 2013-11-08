#include "termios.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"

#define GCRYPT_NO_DEPRECATED
#include <gcrypt.h>

char * getpasswd( char * passwd, char * msg ) {

	/*  disable output for password */
	struct termios old_opts, new_opts;
	tcgetattr( STDIN_FILENO, &old_opts );
	memcpy( &new_opts, &old_opts, sizeof( struct termios ) );
	new_opts.c_lflag &= ~( ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE );
	tcsetattr( STDIN_FILENO, TCSANOW, &new_opts );

	if (msg)
		printf( msg );
	scanf( "%s", passwd );

	/*  reset terminal settings */
	tcsetattr( STDIN_FILENO, TCSANOW, &old_opts );

	if (msg)
		printf( "\n" );

	return passwd;

}

int main( int argc, char** argv ) {

	char passphrase[1024];
	void* salt          = gcry_random_bytes( 8, GCRY_STRONG_RANDOM );
	unsigned char key[33];
	key[32] = 0;

	getpasswd( passphrase, "Enter password: " );

	gcry_kdf_derive( passphrase, strlen(passphrase), GCRY_KDF_ITERSALTED_S2K,
			GCRY_MD_SHA512, salt, 8, 10, 32, key );

	size_t i;
	for (i = 0; i < 32; i++) {
		printf("%02x", key[i]);
	}
	printf("\n");

}
