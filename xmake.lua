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

target('clamshell')
  set_kind('binary')
  set_installdir('/usr')
  add_files('src/**.cc')
  add_packages(
    'xdgcpp',
    'toml++'
  )
  if is_mode("debug") then
    add_defines("DEBUG")
  elseif is_mode("release") then
    add_defines("RELEASE")
  end
  add_defines('TOML_HEADER_ONLY=1')
