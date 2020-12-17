FROM ubuntu:focal

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get -y install \
        automake \
        autotools-dev \
        build-essential \
        bison \
        clang \
        cmake \
        curl \
        debhelper \
        default-jdk \
        devscripts \
        dh-exec \
        dh-lua \
        dh-python \
        discount \
        doxygen \
        flex \
        git \
        git-buildpackage \
        graphviz \
        ghostscript \
        libaugeas-dev \
        libboost-dev \
        libcurl4-gnutls-dev \
        libdbus-1-dev \
        libgit2-dev \
        libgpgme-dev \
        libqt5svg5-dev \
        liblua5.2-dev \
        libmarkdown2-dev \
        libssl-dev \
        libsystemd-dev \
        libxerces-c-dev \
        libxml2-dev \
        libyajl-dev \
        libyaml-cpp-dev \
        libzmq3-dev \
        lintian \
        locales \
        maven \
        pkg-config \
        python-all \
        python-dev \
        python3-all \
        python3-dev \
        ninja-build \
        qtbase5-dev \
        qml-module-qtquick2 \
        qml-module-qtquick-window2 \
        qml-module-qtquick-dialogs \ 
        qml-module-qt-labs-folderlistmodel \
        qml-module-qt-labs-settings \
        qtdeclarative5-dev \
        ruby-dev \
        rustc \
        valgrind \
        libpcre3 \
        libpcre3-dev \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://downloads.sourceforge.net/swig/swig-4.0.2.tar.gz && \
    tar -xvzf swig-4.0.2.tar.gz && \
    cd swig-4.0.2 && \
    ./configure --prefix=/usr \
                --without-maximum-compile-warnings && \
    make && \
    make install && \
    install -v -m755 -d /usr/share/doc/swig-4.0.2 && \
    cp -v -R Doc/* /usr/share/doc/swig-4.0.2

# Google Test
ENV GTEST_ROOT=/opt/gtest
ARG GTEST_VER=release-1.10.0
RUN mkdir -p ${GTEST_ROOT} \
    && cd /tmp \
    && curl -o gtest.tar.gz \
      -L https://github.com/google/googletest/archive/${GTEST_VER}.tar.gz \
    && tar -zxvf gtest.tar.gz --strip-components=1 -C ${GTEST_ROOT} \
    && rm gtest.tar.gz

# Update cache for shared libraries
RUN ldconfig # TODO: mabye move after

RUN ln -s /usr/bin/swig /usr/bin/swig3.0

# Create User:Group
# The id is important as jenkins docker agents use the same id that is running
# on the slaves to execute containers

ARG JENKINS_GROUPID
RUN groupadd \
    -g ${JENKINS_GROUPID} \
    -f \
    jenkins

ARG JENKINS_USERID
RUN useradd \
    --create-home \
    --uid ${JENKINS_USERID} \
    --gid ${JENKINS_GROUPID} \
    --shell "/bin/bash" \
	jenkins


ENV ELEKTRA_ROOT=/opt/elektra/
RUN mkdir -p ${ELEKTRA_ROOT}
COPY ./packages/* ${ELEKTRA_ROOT}

RUN dpkg -i ${ELEKTRA_ROOT}/*

RUN kdb mount-info \
    && mkdir -p `kdb sget system:/info/elektra/constants/cmake/KDB_DB_SPEC .` || true \
    && chown -R ${JENKINS_USERID} `kdb sget system:/info/elektra/constants/cmake/KDB_DB_SPEC .` \
    && chown -R ${JENKINS_USERID} `kdb sget system:/info/elektra/constants/cmake/KDB_DB_SYSTEM .` \
    && chown -R ${JENKINS_USERID} `kdb sget system:/info/elektra/constants/cmake/BUILTIN_DATA_FOLDER .`

USER ${JENKINS_USERID}

# only for testing remove afterwards! FIXME
CMD tail -f /dev/null
