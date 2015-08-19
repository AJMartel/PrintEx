/********************************************************************************	Copyright 2015 Christopher Andrews (http://arduino.land)		Released under MIT licence.********************************************************************************/#ifndef HEADER_MEMORYPRINTER	#define HEADER_MEMORYPRINTER		#include "NonStreamingIO.h"	#include "PrintExtension.h"			class AnyPrinter : public PrintExtension{		public:			AnyPrinter( Print& printObj ) : printObj(printObj) { return; }			size_t write( uint8_t u_Data ){ return printObj.write( u_Data ); }		protected:			friend class Print;		private:			Print &printObj;	};			/***************************************************************************		EEPROMPrinter Class.			An IO class for Print functionality targeting EEPROM memory.	***************************************************************************/#ifdef __AVR__	#include <avr/eeprom.h>	class EEPROMPrinter 						:	public NonStreamingIO, 							public PrintExtension, 							public Printable{		public:			template< typename T > 				EEPROMPrinter( const T t_DataPtr ) : NonStreamingIO( ( uint8_t* ) t_DataPtr ), Printable() 					{ return; }						template< typename T > 				EEPROMPrinter( const T t_DataPtr, const unsigned int u_Length	) :	NonStreamingIO( ( uint8_t* ) t_DataPtr, u_Length ), Printable() 					{ return; }				size_t write( uint8_t u_Data );		protected:			friend class Print;			size_t printTo(Print& p) const;		private:	};#endif	/***************************************************************************		SRAMPrinter Class.			An IO class for Print functionality targeting SRAM memory.	***************************************************************************/		class SRAMPrinter 						:	public NonStreamingIO,							public PrintExtension{		public:			template< typename T > 				SRAMPrinter( const T t_DataPtr ) : NonStreamingIO( ( uint8_t* ) t_DataPtr ) 					{ return; }							template< typename T > 				SRAMPrinter( const T t_DataPtr, const unsigned int u_Length	) : NonStreamingIO( ( uint8_t* ) t_DataPtr, u_Length ) 					{ return; }									operator char*( void ) 				{ 					*End = '\0';					return ( char* ) Start;				}				void end( void ) { *End++ = '\0'; }				size_t write( uint8_t u_Data );		protected:		private:	};		/***************************************************************************		PROGMEMPrinter Class.			This class encapsulates PROGMEM functionality.	***************************************************************************/#ifdef __AVR__	class PROGMEMPrinter : public Printable{		public:			template< typename T > 				PROGMEMPrinter( const T t_DataPtr ) : Start( ( uint8_t* ) t_DataPtr ) 					{ return; }								PROGMEMPrinter &operator =( uint8_t *t )							{ 					Start = t;					return *this; 				}						protected:			friend class Print;			size_t printTo(Print& p) const;		private:			uint8_t *Start;	};#endif	typedef SRAMPrinter			GString;  #ifdef __AVR__	typedef EEPROMPrinter		EString;	typedef PROGMEMPrinter		PString;  #endif#endif//EOF