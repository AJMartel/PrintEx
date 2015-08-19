/********************************************************************************	Copyright 2015 Christopher Andrews (http://arduino.land)		Released under MIT licence.********************************************************************************/	#include "MemoryPrinter.h"	//#if __cplusplus < 201103L || !defined(__GXX_EXPERIMENTAL_CXX0X__)	/*		printf constants.				Define these constants below to disable certain features.				PRINTF_NO_WIDTH_PARAM		PRINTF_NO_PROGMEM		PRINTF_NO_EEPROM		PRINTF_NO_FLOATING_POINT		PRINTF_NO_REPEAT		PRINTF_NO_ERROR_CONDITION	*/		#define CHAR_PERCENT				'%'	#define CHAR_NULL					'\0'	#define CHAR_MINUS					'-'	#define CHAR_ZERO					'0'	#define CHAR_NINE					'9'	#define CHAR_SPACE					' '	#define CHAR_STAR					'*'	#define CHAR_HASH					'#'		#define CHAR_s						's'	#define CHAR_d						'd'	#define CHAR_i						'i'	#define CHAR_F						'F'	#define CHAR_f						'f'	#define CHAR_x						'x'	#define CHAR_X						'X'	#define CHAR_u						'u'	#define CHAR_c						'c'	#define CHAR_l						'l'	#define CHAR_p						'p'	#define CHAR_r						'r'	#define CHAR_n						'n'		#define PAD_RIGHT 					1	#define PAD_ZERO					2			#define PRINTF_CONVERT_BUFFER_LEN	24	#define PRINTF_ERROR_MESSAGE		"Error" //F("Error") may be used also.	const uint32_t GetParam_uint( const va_list &v_List, const bool b_16 )		{			if( b_16 )				return va_arg( v_List, unsigned int );			else				return va_arg( v_List, uint32_t );		}	const int GetParam_int( const va_list &v_List ) __attribute__((noinline));	const int GetParam_int( const va_list &v_List )		{			return va_arg( v_List, int );		}				/****************************************************************		cwrite function.			Helper for printf.	****************************************************************/		inline void PrintExtension::cwrite( uint8_t u_Data, pfct &p_Counter )		{ 			write( u_Data );			++p_Counter;		}					/****************************************************************		printf function.			Prints formatted text.						Formatting options use the following syntax: 				%[flags][width][length]specifier							Flags:				-:	Left-justify within the given field width; Right justification is the default.				0:	When padding is specified, zeros are used instead of spaces.							Width:				(number):	Minimum number of characters to be printed. 							If the value to be printed is shorter than this number,							the result is padded with blank spaces.							The value is not truncated even if the result is larger.														*:	The width is not specified in the format string, 					but as an additional integer value argument preceding 					the argument that has to be formatted.								Extra:									r:	When using 'r' to read the EEPROM the width is the number of 						characters to read.										n:	Number of times to run repeat function.										Length:				l:	d, i use long instead of int.					u, x use unsigned long instead of unsigned int.					n		repeats a string.			Specifier:				s:	String ( null terminated ).				p:	PROGMEM string. No formatting takes place, the string is printed directly.				r:	EEPROM string. No formatting takes place, the string is printed directly.				d:	Signed decimal integer ( 32bits max ).				i:	Same as 'd'.				u:	Unsigned decimal integer ( 32bits max ).				f:	Decimal floating point number.				x:	Unsigned decimal integer ( 32bits max ).				c:	Character.				n:	Repeat function ( default character, see length ).				%:	Escape character for printing '%'	****************************************************************/	pft PrintExtension::printf( const char *format, ... )		{			va_list v_List;			va_start( v_List, format );			const pft p_Return = _printf( format, v_List );			va_end( v_List );			return p_Return;		}				pft PrintExtension::_printf( const char *format, const va_list &v_List )		{			unsigned int		p_Width;			pfct				p_Pad;			pfct				p_Counter 		= 0x00;			unsigned char 		buffer[ PRINTF_CONVERT_BUFFER_LEN ];			//Buffer for conversions.						for( ; *format ; ++format ){							if( *format == CHAR_PERCENT ){									++format;					p_Width = p_Pad = 0x00;										bool b_LargeType = false;										if( *format == CHAR_NULL )						--format;					else if( *format == CHAR_PERCENT )						cwrite( *format, p_Counter );					else{											//Check for left justify.						if( *format == CHAR_MINUS ){							++format;							p_Pad = PAD_RIGHT;							}													//Check for '0' pad specifier.						if( *format == CHAR_ZERO ){							++format;							p_Pad |= PAD_ZERO;						}												//Check for passed length.						#ifndef PRINTF_NO_WIDTH_PARAM 							if( *format == CHAR_STAR ){								++format;								p_Width = ( unsigned int ) GetParam_int( v_List );//va_arg( v_List, int );							//Width provided in format specifier.							}else						#endif												//Calculate padding width.						for ( ; *format >= CHAR_ZERO && *format <= CHAR_NINE; ++format ){							p_Width *= 10;							p_Width += *format - CHAR_ZERO;						}												char	c_PadChar = ( p_Pad & PAD_ZERO ) ? CHAR_ZERO : CHAR_SPACE;						GString g_Convert( buffer );												//Get length flag if for larger types.						if( *format == CHAR_l ){							b_LargeType = true;							++format;						}												//Convert argument into a string.						if( *format == CHAR_s ){													//String.							char *c_Parameter = ( char* ) GetParam_int( v_List );							g_Convert.assign( c_Parameter, strlen( c_Parameter ) );													#ifndef PRINTF_NO_PROGMEM							}else if( *format == CHAR_p ){								print( PString( ( void* ) GetParam_int( v_List ) ) );								continue;						#endif												#ifndef PRINTF_NO_EEPROM							}else if( *format == CHAR_r ){								print( EString( ( void* ) GetParam_int( v_List ), p_Width ) );								continue;						#endif													#ifndef PRINTF_NO_REPEAT							}else if( *format == CHAR_n ){								const int i_Data = GetParam_int( v_List );								while( p_Width-- ){									if( b_LargeType )										print( ( char* ) i_Data );									else										print( ( char ) i_Data );								}								continue;						#endif												}else if( *format == CHAR_d ||  *format == CHAR_i ){ 						//Signed decimal integer.							if( b_LargeType )	g_Convert.print( va_arg( v_List, int32_t ), DEC );							else				g_Convert.print( ( int32_t ) GetParam_int( v_List ), DEC );													}else if( *format == CHAR_u ){												//Unsigned integer.							if( b_LargeType )	g_Convert.print( GetParam_uint( v_List, false ), DEC );							else				g_Convert.print( ( uint32_t ) GetParam_uint( v_List, true ), DEC );													#ifndef PRINTF_NO_FLOATING_POINT							}else if( *format == CHAR_f ){											//Floating point data.								g_Convert.print( va_arg( v_List, double ) );						#endif												}else if( *format == CHAR_x ){												//Unsigned hexidecimal integer.							if( b_LargeType )	g_Convert.print( GetParam_uint( v_List, false ), HEX );							else				g_Convert.print( ( uint32_t ) GetParam_uint( v_List, true ), HEX ); 													}else if(  *format == CHAR_c ){												//Character.							*buffer = ( char ) GetParam_int( v_List );							g_Convert.assign( buffer, 1 ); //This is cheaper than printing.													#if ( defined( PRINTF_NO_EEPROM ) || defined( PRINTF_NO_FLOATING_POINT ) || defined( PRINTF_NO_PROGMEM ) ) && !defined( PRINTF_NO_ERROR_CONDITION )							}else								g_Convert.print( PRINTF_ERROR_MESSAGE );						#else							}						#endif						//Calculate remaining padding.						const pft p_Len = g_Convert.count();						if( p_Len >= p_Width )	p_Width = 0x00;						else					p_Width -= p_Len;						//Pad left if right justified.						if( !( p_Pad & PAD_RIGHT ) ){							repeat( c_PadChar, p_Width );							p_Counter += p_Width;							p_Width = 0;						}												p_Counter += write( ( unsigned char* ) ( char* ) g_Convert, p_Len );						//Pad right if left justified.						repeat( c_PadChar, p_Width );							p_Counter += p_Width;					}				}else //Standard character, write out.					cwrite( *format, p_Counter );			}			return p_Counter;		}				//#endif	/****************************************************************		repeat function.			Prints c_Character, repeating u_Count times.	****************************************************************/		void PrintExtension::repeat( const char &c_Character, unsigned char u_Count ) { while( u_Count-- ) write( c_Character ); }					//EOF	