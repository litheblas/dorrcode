#define LOCAL_I2C_ADR 0x02
#define DOOR_I2C_ADR 0x04


//i2c_commands
//to computer
#define SEND_TO_COMPUTER 's'
#define TELL_DOOR_OPEN_COMMAND 't'
#define TELL_DOOR_CLOSE_COMMAND 'r'

//to MCU
#define ASK_DOOR_STATUS_COMMAND 'y' 
#define OPEN_DOOR_COMMAND 'o'
#define CLOSE_DOOR_COMMAND 'c'
/*****************************************************************
 *---------------------------------------------------------------*
 *****************************************************************/
//rs232 commands
#define ECHO 'a'

//to mcu
#define OPEN_DOOR 'o' //only works to door mcu
#define CLOSE_DOOR 'c'
#define ASK_DOOR_STATUS 'y'
#define SEND_TO_I2C 'p'

//to computer
#define TELL_DOOR_OPEN 't'
#define TELL_DOOR_CLOSED 'r'
