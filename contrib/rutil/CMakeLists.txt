
set (HEADERS    AbstractFifo.hxx
                AndroidLogger.hxx
                AsyncID.hxx
                AsyncProcessHandler.hxx
                BaseException.hxx
                CircularBuffer.hxx
                Coders.hxx
                compat.hxx
                Condition.hxx
                ConfigParse.hxx
                CongestionManager.hxx
                ConsumerFifoBuffer.hxx
                CountStream.hxx
                Data.hxx
                DataException.hxx
                DataStream.hxx
                DinkyPool.hxx
                DnsUtil.hxx
                FdPoll.hxx
                FdSetIOObserver.hxx
                Fifo.hxx
                FileSystem.hxx
                FiniteFifo.hxx
                GeneralCongestionManager.hxx
                GenericIPAddress.hxx
                GenericTimerQueue.hxx
                HashMap.hxx
                HeapInstanceCounter.hxx
                Inserter.hxx
                IntrusiveListElement.hxx
                KeyValueStore.hxx
                Lock.hxx
                Lockable.hxx
                Log.hxx
                Logger.hxx
                MD5Stream.hxx
                Mutex.hxx
                NetNs.hxx
                ParseBuffer.hxx
                ParseException.hxx
                Poll.hxx
                PoolBase.hxx
                ProducerFifoBuffer.hxx
                RADIUSDigestAuthenticator.hxx
                Random.hxx
                RecursiveMutex.hxx
                resipfaststreams.hxx
                RWMutex.hxx
                SelectInterruptor.hxx
                ServerProcess.hxx
                Sha1.hxx
                SharedCount.hxx
                SharedPtr.hxx
                Socket.hxx
                StlPoolAllocator.hxx
                Subsystem.hxx
                SysLogBuf.hxx
                SysLogStream.hxx
                ThreadIf.hxx
                Time.hxx
                TimeLimitFifo.hxx
                Timer.hxx
                TransportType.hxx
                vmd5.hxx
                vthread.hxx
                WinCompat.hxx
                WinLeakCheck.hxx
                XMLCursor.hxx
                dns/AresCompat.hxx
                dns/AresDns.hxx
                dns/DnsAAAARecord.hxx
                dns/DnsCnameRecord.hxx
                dns/DnsHandler.hxx
                dns/DnsHostRecord.hxx
                dns/DnsNaptrRecord.hxx
                dns/DnsResourceRecord.hxx
                dns/DnsSrvRecord.hxx
                dns/DnsStub.hxx
                dns/DnsThread.hxx
                dns/ExternalDns.hxx
                dns/ExternalDnsFactory.hxx
                dns/LocalDns.hxx
                dns/QueryTypes.hxx
                dns/RRCache.hxx
                dns/RRFactory.hxx
                dns/RRList.hxx
                dns/RROverlay.hxx
                dns/RRVip.hxx)

set (SOURCES    AbstractFifo.cxx
                AndroidLogger.cxx
                BaseException.cxx
                Coders.cxx
                Condition.cxx
                ConfigParse.cxx
                CountStream.cxx
                Data.cxx
                DataStream.cxx
                DnsUtil.cxx
                FdPoll.cxx
                FileSystem.cxx
                GeneralCongestionManager.cxx
                HeapInstanceCounter.cxx
                KeyValueStore.cxx
                Lock.cxx
                Log.cxx
                MD5Stream.cxx
                Mutex.cxx
                NetNs.cxx
                ParseBuffer.cxx
                ParseException.cxx
                Poll.cxx
                PoolBase.cxx
                RADIUSDigestAuthenticator.cxx
                Random.cxx
                RecursiveMutex.cxx
                resipfaststreams.cxx
                RWMutex.cxx
                SelectInterruptor.cxx
                ServerProcess.cxx
                Sha1.cxx
                Socket.cxx
                Subsystem.cxx
                SysLogBuf.cxx
                SysLogStream.cxx
                ThreadIf.cxx
                Time.cxx
                Timer.cxx
                TransportType.cxx
                vmd5.cxx
                XMLCursor.cxx
                dns/AresDns.cxx
                dns/DnsAAAARecord.cxx
                dns/DnsCnameRecord.cxx
                dns/DnsHostRecord.cxx
                dns/DnsNaptrRecord.cxx
                dns/DnsResourceRecord.cxx
                dns/DnsSrvRecord.cxx
                dns/DnsStub.cxx
                dns/DnsThread.cxx
                dns/ExternalDnsFactory.cxx
                dns/LocalDns.cxx
                dns/QueryTypes.cxx
                dns/RRCache.cxx
                dns/RRList.cxx
                dns/RROverlay.cxx
                dns/RRVip.cxx)
if(USE_OPENSSL)
    add_definitions(-DUSE_SSL)
    set (HEADERS    ${HEADERS}
                    DigestStream.hxx
                    ssl/OpenSSLInit.hxx
                    ssl/SHA1Stream.hxx)
    set (SOURCES    ${SOURCES}
                    DigestStream.cxx
                    ssl/OpenSSLInit.cxx
                    ssl/SHA1Stream.cxx)
endif()

set(ARES_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ares)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/contrib" ${ARES_INCLUDE_DIR})
#include_directories(SYSTEM ${LIBOPENSSL_INCLUDE_DIR})

link_directories(${LIBOPENSSL_LIB_DIR})

add_definitions(-DUSE_ARES)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set (SOURCES     ${SOURCES}
                     "WinCompat.cxx")
endif()

add_library(rutil STATIC ${HEADERS} ${SOURCES})

if(USE_OPENSSL)
    target_link_libraries(rutil ${LIBOPENSSL_LIBS})
endif()
target_link_libraries(rutil ares dl)

target_include_directories(rutil PRIVATE ${LIBOPENSSL_INCLUDE_DIR})

add_subdirectory(ares)
