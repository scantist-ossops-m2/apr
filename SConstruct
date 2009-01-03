#!/usr/bin/env scons
#

from build import APREnv

EnsureSConsVersion(1, 1, 0)

vars = Variables('build.py')

vars.Add('maintainer_mode', 'Turn on debugging and compile time warnings', 0)
vars.Add('profile', 'Turn on profiling for the build (GCC)', 0)
vars.Add(EnumVariable('pool_debug', 'Turn on pools debugging', 'no',
                      allowed_values=('yes', 'no', 'verbose', 'verbose-alloc', 'lifetime', 'owner', 'all')))

env = APREnv(args=ARGUMENTS, variables=vars)

Help(vars.GenerateHelpText(env))

env.APRHints()

env.APRAutoconf()

if env['maintainer_mode']:
  if env.is_gcc():
    env.AppendUnique(CPPFLAGS = ['-g', '-Wall', '-Wmissing-prototypes', '-Wstrict-prototypes', '-Wmissing-declarations'])

if env['profile']:
  env.Filter(CPPFLAGS = '-g')
  env.AppendUnique(CPPFLAGS = ['-pg'])

if env['pool_debug'] != 'no':
  flags = {'yes': 1,
           'verbose': 2,
           'lifetime': 4,
           'owner': 8,
           'verbose-alloc': 16,
            'all': 31}
  env.AppendUnique(CPPFLAGS = "-DAPR_POOL_DEBUG=%d" % flags[env['pool_debug']])

files = env.core_lib_files()

(major, minor, patch) = env.APRVersion()

libapr = env.SharedLibrary('apr-%d' % (major), files)

targets = [libapr]

env.Default(targets)