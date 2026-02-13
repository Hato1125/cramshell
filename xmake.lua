add_rules('mode.debug', 'mode.release')

option('compile_commands')
  set_default(false)
  set_showmenu(true)
  set_description('Generate compile_commands.json')
option_end()

if has_config('compile_commands') then
  ---@diagnostic disable-next-line: param-type-mismatch
  add_rules('plugin.compile_commands.autoupdate', { outputdir = 'build' })
end

set_allowedplats('linux')
set_languages('c++26')

target('cramshell')
  set_kind('binary')
  add_files('src/**.cc')
