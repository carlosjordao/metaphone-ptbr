from distutils.core import setup, Extension

module1 = Extension('metaphoneptbr',
                    sources = ['metaphone_python.c'])

setup (name = 'Metaphone_pt-BR',
       version = '1.0',
       description = 'Metaphone applied for Brazilian Portuguese. This should be more accurated for portuguese words then traditional metaphone and double metaphone algorithms',
       author = 'Carlos C. Jordao',
       author_email = 'carlosjordao@gmail.com',
       url = 'http://sourceforge.net/projects/metaphoneptbr',
       ext_modules = [module1])
