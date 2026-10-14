SECTIONS
{
  .tinydesk_psram (NOLOAD) :
  {
    . = ALIGN(8);
    __tinydesk_psram_start__ = .;
    *(.tinydesk_psram)
    *(.tinydesk_psram.*)
    . = ALIGN(8);
    __tinydesk_psram_end__ = .;
  } > psram
}
