#ifndef COMM_H
#define COMM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
 * @brief General
 */
typedef struct __attribute__((__packed__)) com_def_s
{
    uint8_t u8Heading;
    uint8_t u8Command;
    uint16_t u16PayloadLength;
    // PAYLOAD
    // CRC
    // END
} ComDef_TypeDef;

/**
 * @brief IMU
 */
typedef struct __attribute__ ((__packed__ )) com_def_imu_s
{
    float fX;
    float fY;
    float fZ;
} ComDefImu_TypeDef;

/**
 * @brief Angle
 */
typedef struct __attribute__((__packed__)) com_def_angle_s
{
    uint16_t u16Angle;
} ComDefAngle_TypeDef;




/**
 * @brief getSetMask
 */
typedef enum com_def_command_mask_e
{
    CommandModeMaskGet  = 0x01,
    CommandModeMaskSet  = 0x02,
} CommandModeMask_TypeDef;


/**
 * @brief Maskable Commands
 */
typedef enum com_def_commands_e
{
    ComDefCommandMode   = 0x10,
    ComDefCommandAngle  = 0x20,
} ComDefCommands_TypeDef;

#define ComDef_xu8CommandModeGet(__COMMAND)  \
  ( (__COMMAND) | CommandModeMaskGet )

#define ComDef_xu8CommandModeSet(__COMMAND)  \
  ( (__COMMAND) | CommandModeMaskGet )

// typedef struct __attribute__((__packed__)) com_def_s
// {
//     uint8_t u8Heading;
//     uint8_t u8Command;
//     uint16_t u16PayloadLength;
//     // PAYLOAD
//     // CRC
//     // END
// } ComDef_TypeDef;

#define ComDef_xu8GetHeading(__MSG__)  \
( ( (ComDef_TypeDef*) ((__MSG__)) ) ->u8Heading )

#define ComDef_xu8GetCommand(__MSG__)  \
( ( (ComDef_TypeDef*) ((__MSG__)) )->u8Command )

#define ComDef_xu16GetPayloadLength(__MSG__)  \
( ( (ComDef_TypeDef*) ((__MSG__)) )->u16PayloadLength )

#define ComDef_xu8GetCrc(__MSG__)  \
( * (  ( (uint8_t*)((__MSG__)) ) + ComDef_xu16GetPayloadLength(__MSG__) +3 ) )

#define ComDef_xu8GetEnd(__MSG__)  \
( * ( ( (uint8_t*)((__MSG__)) ) + ComDef_xu16GetPayloadLength(__MSG__) +4 ) )

#define ComDef_xpu8DeclareBuffer(__BUFFERNAME, __PAYLOADTYPE) \
uint8_t __BUFFERNAME [sizeof( __PAYLOADTYPE) + 5] = {0}

#define ComDef_xpu8GetPayload(__MSG__)  \
( * ( ( (uint8_t *)&( ComDef_xu16GetPayloadLength(__MSG__)) ) +1) )





#ifdef __cplusplus
}
#endif

#endif // COMM_H
