FROM fuzzers/afl:2.52

RUN apt-get update
RUN apt install -y build-essential wget git clang cmake  automake autotools-dev  libtool zlib1g zlib1g-dev
RUN git clone https://github.com/team-charls/charls.git
WORKDIR /charls
RUN cmake -DCMAKE_C_COMPILER=afl-clang -DCMAKE_CXX_COMPILER=afl-clang++ .
RUN make
RUN make install
RUN cp ./samples/convert.c/convert-c /convert-c
# Only bmp 24 bits as corpus
RUN mkdir /charlsCorpus
RUN wget https://people.math.sc.edu/Burkardt/data/bmp/bmp_24.bmp
RUN mv *.bmp /charlsCorpus
ENV LD_LIBRARY_PATH=/usr/local/lib

ENTRYPOINT ["afl-fuzz", "-i", "/charlsCorpus", "-o", "/charlsOut"]
CMD ["/charls/samples/convert.c/convert-c", "@@", "out.png"]
