
include_directories("${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/contrib")

set (HEADERS    ares.h
                ares_compat.h
                ares_dns.h
                ares_local.h
                ares_private.h
                ares_socketfunc.h
                ares_version.h)

set (SOURCES    ares__close_sockets.c
                ares__get_hostent.c
                ares__read_line.c
                ares_destroy.c
                ares_expand_name.c
                ares_fds.c
                ares_free_errmem.c
                ares_free_hostent.c
                ares_free_string.c
                ares_gethostbyaddr.c
                ares_gethostbyname.c
                ares_init.c
                ares_local.c
                ares_mkquery.c
                ares_parse_a_reply.c
                ares_parse_ptr_reply.c
                ares_process.c
                ares_query.c
                ares_search.c
                ares_send.c
                ares_strerror.c
                ares_timeout.c)

add_library(ares STATIC ${HEADERS} ${SOURCES})

#install(FILES ${HEADERS} DESTINATION include)
#install(TARGETS ares DESTINATION lib)