add_rules('mode.debug', 'mode.release')

add_rules('plugin.compile_commands.autoupdate', {
  outputdir = 'build'
})

set_languages('c++26')

target('cramshell')
  set_kind('binary')
  add_files('**.cc')
