SECTIONS
{
  .miniface_psram (NOLOAD) :
  {
    . = ALIGN(8);
    __miniface_psram_start__ = .;
    *(.miniface_psram)
    *(.miniface_psram.*)
    . = ALIGN(8);
    __miniface_psram_end__ = .;
  } > psram
}
