add_rules('mode.debug', 'mode.release')

package('xdgcpp')
  set_urls('https://github.com/Grumbel/xdgcpp.git')
  add_deps('cmake')

  on_install(
    function (package)
      import('package.tools.cmake').install(package)
    end
  )
package_end()

add_requires('xdgcpp')
add_requires('toml++')

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

target('clamshell-daemon')
  set_kind('binary')
  add_files('src/daemon/**.cc')
  add_packages(
    'xdgcpp',
    'toml++'
  )
  add_includedirs('src')
  add_defines('TOML_HEADER_ONLY=1')


target('clamshell-agent')
  set_kind('binary')
  add_files('src/agent/**.cc')
  add_packages(
    'xdgcpp',
    'toml++'
  )
  add_includedirs('src')
  add_defines('TOML_HEADER_ONLY=1')
