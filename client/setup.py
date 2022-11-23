#!/usr/bin python3

from distutils.core import setup

setup(name='client',
      version='1.0',
      description='client for net simulation',
      author='Brandon Shimanek',
      author_email='na',
      packages=['client'],
      install_requires=['requests'],
      entry_points={
          'console_scripts':
          ['net-client = client.__main__:main'],
      }
      )
