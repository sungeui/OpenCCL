cmake_minimum_required(VERSION 3.0)
project(metis)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_BUILD_TOOL Release)

set(CMAKE_C_FLAGS "-O2")

include_directories(.)

add_library(metis
        coarsen.c
        fm.c
        initpart.c
        match.c
        ccgraph.c
        memory.c
        pmetis.c
        pqueue.c
        refine.c
        util.c
        timing.c
        debug.c
        bucketsort.c
        graph.c
        stat.c
        kmetis.c
        kwayrefine.c
        kwayfm.c
        balance.c
        ometis.c
        srefine.c
        sfm.c
        separator.c
        mincover.c
        mmd.c
        mesh.c
        meshpart.c
        frename.c
        fortran.c
        myqsort.c
        compress.c
        parmetis.c
        estmem.c
        mpmetis.c
        mcoarsen.c
        mmatch.c
        minitpart.c
        mbalance.c
        mrefine.c
        mutil.c
        mfm.c
        mkmetis.c
        mkwayrefine.c
        mkwayfmh.c
        mrefine2.c
        minitpart2.c
        mbalance2.c
        mfm2.c
        kvmetis.c
        kwayvolrefine.c
        kwayvolfm.c
        subdomains.c)

install(TARGETS metis DESTINATION ../../Lib)