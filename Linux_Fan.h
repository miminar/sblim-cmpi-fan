#ifndef LINUX_FAN_H_
#define LINUX_FAN_H_

#include <stdbool.h>

/** accessible features of fan
 * each value represents a bit position in accessible_features vector in
 * cim_fan struct
 */
#define CIM_FAN_AF_MIN_SPEED   (1 << 0)
#define CIM_FAN_AF_MAX_SPEED   (1 << 1)
#define CIM_FAN_AF_DIV         (1 << 2)
#define CIM_FAN_AF_PULSES      (1 << 3)
#define CIM_FAN_AF_FAULT       (1 << 4)
#define CIM_FAN_AF_BEEP        (1 << 5)
#define CIM_FAN_AF_ALARM       (1 << 6)
#define CIM_FAN_AF_ALARM_MIN   (1 << 7)
#define CIM_FAN_AF_ALARM_MAX   (1 << 8)
#define CIM_FAN_AF_FEATURE_MAX (1 << 8)

/**
 * Descriptions of attributes are taken from:
 * sysfs-interface documentation of hwmon 
 */
struct cim_fan{
    char const * chip_name;
    char const * sys_path;
    char const * name;
    /* is a composition of sys_path and name
     * uniquely identifies fan
     */
    char const * device_id;

    /* This is a bit vector with bit on position i meaning:
     *  CIM_FAN_AF_[i] = 1 <==> feature is available in fan's kernel interface
     *                          (a file/channel is available under sys_path)
     */
    unsigned int accessible_features;

    /* [RO] Fan input value.
     * Unit: revolution/min (RPM)
     */
    unsigned int speed;

    /* [RW] Fan minimum value
     * Unit: revolution/min (RPM)
     */
    unsigned int min_speed;

    /* [RW] Fan maximum value
     * Unit: revolution/min (RPM)
     * Only rarely supported by the hardware.
     */
    unsigned int max_speed;

    /* [RW] Fan divisor.
     * Integer value in powers of two (1, 2, 4, 8, 16, 32, 64, 128).
     * Some chips only support values 1, 2, 4 and 8.
     * Note that this is actually an internal clock divisor, which
     * affects the measurable speed range, not the read value.
     */
    unsigned int divisor;

    /* [RW] Number of tachometer pulses per fan revolution.
     * Integer value, typically between 1 and 4.
     * This value is a characteristic of the fan connected to the
     * device's input, so it has to be set in accordance with the fan
     * model.
     * Should only be created if the chip has a register to configure
     * the number of pulses. In the absence of such a register (and
     * thus attribute) the value assumed by all devices is 2 pulses
     * per fan revolution.
     */
    unsigned int pulses;

    /* [RO] Each input channel may have an associated fault file. This can be
     * used to notify open diodes, unconnected fans etc. where the hardware
     * supports it. When this boolean has value 1, the measurement for that
     * channel should not be trusted.
     */
    bool fault;

    /* [RW] Channel beep
     * false : disable
     * true  : enable
     * Some chips also offer the possibility to get beeped when an alarm
     * occurs.
     */
    bool beep;

    /* [RO] Channel alarm
     * false : no alarm
     * true  : alarm
     * Each channel or limit may have an associated alarm file, containing a
     * boolean value. 1 means that an alarm condition exists, 0 means no alarm.
     * 
     * Usually a given chip will either use channel-related alarms, or
     * limit-related alarms, not both. The driver should just reflect the
     * hardware implementation.
     */
    bool alarm;
    bool alarm_min;
    bool alarm_max;
};

struct fanlist{
    struct cim_fan * f;
    struct fanlist * next;
};

typedef enum cim_fan_prop_type{
    CIM_FAN_MIN_SPEED = 1 << 0,
    CIM_FAN_MAX_SPEED = 1 << 1,
    CIM_FAN_DIV       = 1 << 2,
    CIM_FAN_PULSES    = 1 << 3,
    CIM_FAN_BEEP      = 1 << 4,
}cim_fan_prop_type_t;

typedef union cim_fan_prop_value{
    unsigned int min_speed;
    unsigned int max_speed;
    unsigned int divisor;
    unsigned int pulses;
    bool beep;
}cim_fan_prop_value_t;

/** errors *******************************************************************/
typedef enum cim_fan_error{
    CIM_FAN_SUCCESS                = 0,
    CIM_FAN_FAILED_TO_GET_FAN      = 1,
    CIM_FAN_MISSING_IDENTIFICATION = 2,
    CIM_FAN_NO_SUCH_CHIP           = 3,
    CIM_FAN_NO_SUCH_FAN_NAME       = 4,
    CIM_FAN_UNKNOWN_PROP           = 5,
    CIM_FAN_NOT_SUPPORTED_PROP     = 6,
    CIM_FAN_NOT_WRITABLE_PROP      = 7,
    CIM_FAN_INIT_FAILED            = 9,
    CIM_FAN_OTHER_ERROR            = 10,

    CIM_FAN_SEN_WILDCARDS          = 0x101,
    CIM_FAN_SEN_NO_ENTRY           = 0x102,
    CIM_FAN_SEN_ACCESS_R           = 0x103,
    CIM_FAN_SEN_KERNEL             = 0x104,
    CIM_FAN_SEN_DIV_ZERO           = 0x105,
    CIM_FAN_SEN_CHIP_NAME          = 0x106,
    CIM_FAN_SEN_BUS_NAME           = 0x107,
    CIM_FAN_SEN_PARSE              = 0x108,
    CIM_FAN_SEN_ACCESS_W           = 0x109,
    CIM_FAN_SEN_IO                 = 0x110,
    CIM_FAN_SEN_RECURSION          = 0x111,
}cim_fan_error_t;

char const * cim_fan_strerror(cim_fan_error_t error);

cim_fan_error_t enum_all_fans(struct fanlist ** lptr);

cim_fan_error_t get_fan_data_by_id(
        char const *device_id,
        struct cim_fan **sptr);
cim_fan_error_t get_fan_data(
        char const *sys_path,
        char const *fan_name,
        struct cim_fan **sptr);

cim_fan_error_t set_fan_prop_by_id(
        char const *device_id,
        cim_fan_prop_type_t prop_type,
        cim_fan_prop_value_t const *prop);
cim_fan_error_t set_fan_prop(
        char const *sys_path,
        char const *fan_name,
        cim_fan_prop_type_t prop_type,
        cim_fan_prop_value_t const *prop);

void free_fanlist(struct fanlist *lptr);
void free_fan(struct cim_fan *sptr);

cim_fan_error_t init_linux_fan_module();
void cleanup_linux_fan_module();

#endif /* ----- LINUX_FAN_H_ ----- */

