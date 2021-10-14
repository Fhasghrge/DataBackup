FROM centos

RUN yum install -y boost \
&&  yum install -y jsoncpp-1.8.4-6.el8.x86_64 \
&&  yum install -y openssl-devel \
&&  yum install -y git

RUN git clone https://github.com/zaphoyd/websocketpp \
&&  cd websocketcpp \
&&  mkdir build \
&&  cd build \
&&  cmake .. \
&&  make \
&&  make install

COPY *.hpp ./
COPY *.cpp ./
COPY build.sh ./Filebuild.sh 
 
RUN chmod 775 ./Filebuild.sh

EXPOSE 9003 3000
ENTRYPOINT ["Filebuild.sh"] 
