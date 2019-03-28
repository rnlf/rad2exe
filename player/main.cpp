#include <i86.h>
#include <conio.h>
#include <stdint.h>

#define RAD_DETECT_REPEATS 1
#include "player20.cpp"

#define OPL_BASE_REG                0x388U

#define PIT_CHANNEL0_DATA_REGISTER  0x40U
#define PIT_MODE_COMMAND_REGISTER   0x43U
#define PIT_MODE_RATE_GENERATOR     0x04U
#define PIT_BASE_FREQUENCY          1193182UL 
#define PIT_CHANNEL0_INTERRUPT      8U
#define PIT_MODE_MASK               0x3FU
#define PIT_RESET_RELOAD            0xFFFFU
#define PIT_CHANNEL0_RATE_GEN_MODE  0x34U

#define MASTER_PIC_COMMAND_REGISTER 0x20U
#define PIC_EOI                     0x20U

#define LOBYTE(v) ((v) & 0xFFU)
#define HIBYTE(v) (((v) & 0xFF00U) >> 8U)

#include <songdata.h>


static RADPlayer player;
static volatile bool repeating = false;


static void _interrupt pit_isr() {
  repeating = player.Update();
  outp(MASTER_PIC_COMMAND_REGISTER, PIC_EOI);
}


static void pit_set_reload(unsigned const reload) {
  outp(PIT_CHANNEL0_DATA_REGISTER, static_cast<uint8_t>(LOBYTE(reload)));
  outp(PIT_CHANNEL0_DATA_REGISTER, static_cast<uint8_t>(HIBYTE(reload)));
}


typedef void _interrupt (__far *ISR_PTR)();

struct IvtEntry {
  uint16_t off;
  uint16_t seg;
};

static IvtEntry volatile __far * const ivt = reinterpret_cast<IvtEntry volatile __far*>(MK_FP(0,0));

static void install_isr(int const vect, ISR_PTR const isr) {
  ivt[vect].seg = FP_SEG(isr);
  ivt[vect].off = FP_OFF(isr);
}


static ISR_PTR get_isr(int const vect) {
  return reinterpret_cast<ISR_PTR>(MK_FP(ivt[vect].seg, ivt[vect].off));
}


static uint8_t original_pit_mode;
static ISR_PTR original_pit_isr;

static void initialize_pit(unsigned long const frequency_hz) {
  _disable();

  original_pit_mode = inp(PIT_CHANNEL0_DATA_REGISTER);
  outp(PIT_MODE_COMMAND_REGISTER, PIT_CHANNEL0_RATE_GEN_MODE);

  unsigned long const reload = PIT_BASE_FREQUENCY / frequency_hz;

  pit_set_reload(reload);

  original_pit_isr = get_isr(PIT_CHANNEL0_INTERRUPT);
  install_isr(PIT_CHANNEL0_INTERRUPT, &pit_isr);

  _enable();
}


static void cleanup_pit() {
  _disable();
  install_isr(PIT_CHANNEL0_INTERRUPT, original_pit_isr);
  outp(PIT_MODE_COMMAND_REGISTER, original_pit_mode & PIT_MODE_MASK);
  pit_set_reload(PIT_RESET_RELOAD);
  _enable();
}


static inline void delay_idx() {
  inp(OPL_BASE_REG);
}


static inline void delay_data() {
  inp(OPL_BASE_REG);
  inp(OPL_BASE_REG);
  inp(OPL_BASE_REG);
}


static inline void write_reg(uint16_t const base, uint8_t const reg, uint8_t const val) {
  outp(base,   reg);
  delay_idx();
  outp(base+1, val);
  delay_data();
}


static void write_opl_reg(void*, uint16_t const reg, uint8_t const val) {
  uint16_t const base_reg_offset = ((reg >> 7U) & 2U); // 2 if reg >= 0x100, 0 otherwise
  write_reg(
    OPL_BASE_REG | base_reg_offset,
    static_cast<uint8_t>(LOBYTE(reg)),
    val
  );
}


int main(int argc, char** argv) {
  player.Init(music_data, &write_opl_reg, reinterpret_cast<void*>(0));

  initialize_pit(player.GetHertz());

  while(!repeating) {
    if(kbhit()) {
      getch();
      break;
    }
  }

  cleanup_pit();
  player.Stop();
}
