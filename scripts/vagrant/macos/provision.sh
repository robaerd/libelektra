#!/bin/zsh

# install dependencies
brew update
brew config
brew install augeas \
  bison \
  cmake \
  dbus \
  discount \
  flex \
  gpgme \
  libev \
  libgcrypt \
  libgit2 \
  libuv \
  lua \
  ninja \
  swig \
  yajl \
  zeromq
brew install --cask adoptopenjdk

## post install
sudo gem install test-unit --no-document
brew tap homebrew/services
brew update # Work around for [Homebrew Services issue 206](https://github.com/Homebrew/homebrew-services/issues/206)
brew postinstall dbus
brew services restart dbus
cmake -E make_directory $HOME/elektra/build
