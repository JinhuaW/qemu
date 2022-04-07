#include "qemu/osdep.h"
#include "hw/hw.h"
#include "hw/sysbus.h"
#include "qemu/bitops.h"
#include "qemu/log.h"

#define TYPE_VIRT_CPLD          "virt-cpld"
#define VIRT_CPLD(obj)          OBJECT_CHECK(VirtCpldState, (obj), TYPE_VIRT_CPLD)

/* Register map */
#define ID_RESET_REASON        0x4
#define ID_BOARD_TYPE          0x80


typedef struct {
    SysBusDevice parent_obj;
    MemoryRegion iomem;
    uint32_t init;
    uint32_t memory[0x200];
} VirtCpldState;

static uint64_t virt_cpld_read(void *opaque, hwaddr offset, unsigned size)
{
    VirtCpldState *s = (VirtCpldState *)opaque;

    if (offset >= 0x800) {
        fprintf(stderr, "CPLD read offset out of range\n");
        return 0;
    }

    switch (offset) {
    case ID_RESET_REASON:
        return 1;
    case ID_BOARD_TYPE:
        return 0x20;
    default:
	return s->memory[offset>>2];
    }
}

static void virt_cpld_write(void *opaque, hwaddr offset, uint64_t value,
                          unsigned size)
{
    VirtCpldState *s = (VirtCpldState *)opaque;

    if (offset >= 0x800) {
        fprintf(stderr, "CPLD write offset out of range\n");
        return;
    }
    switch (offset) {
    case ID_RESET_REASON:
    case ID_BOARD_TYPE:
        break;
    default:
	s->memory[offset>>2] = value;
    }
}

static const MemoryRegionOps virt_cpld_ops = {
    .read = virt_cpld_read,
    .write = virt_cpld_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void virt_cpld_realize(DeviceState *d, Error **errp)
{
    VirtCpldState *s = VIRT_CPLD(d);
    SysBusDevice *sbd = SYS_BUS_DEVICE(d);

    memory_region_init_io(&s->iomem, OBJECT(s), &virt_cpld_ops, s,
                          TYPE_VIRT_CPLD, 0x800);
    sysbus_init_mmio(sbd, &s->iomem);
}

static void virt_cpld_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = virt_cpld_realize;
}

static const TypeInfo virt_cpld_info = {
    .name          = TYPE_VIRT_CPLD,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(VirtCpldState),
    .class_init    = virt_cpld_class_init,
};

static void virt_cpld_register_types(void)
{
    type_register_static(&virt_cpld_info);
}

type_init(virt_cpld_register_types)
