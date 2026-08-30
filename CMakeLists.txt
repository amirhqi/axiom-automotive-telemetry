cmake_minimum_required(VERSION 3.21)

project(AutomotiveTelemetryConsole VERSION 2.6.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer on GCC/Clang" OFF)
option(BUILD_BENCHMARKS "Build parser benchmark" OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Network SerialPort)
qt_standard_project_setup(REQUIRES 6.5)

function(configure_project_target target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        if(ENABLE_SANITIZERS AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
            target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${target_name} PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
endfunction()

qt_add_executable(AutomotiveTelemetryConsole
    src/main.cpp
    src/TelemetryFrame.h
    src/PacketParser.h
    src/PacketParser.cpp
    src/CsvLogger.h
    src/CsvLogger.cpp
    src/TelemetryWorker.h
    src/TelemetryWorker.cpp
    src/TelemetryController.h
    src/TelemetryController.cpp
)


qt_add_resources(AutomotiveTelemetryConsole "axiom_branding"
    PREFIX "/"
    FILES
        assets/axiom_icon.png
        assets/axiom_logo.png
)

set_target_properties(AutomotiveTelemetryConsole PROPERTIES
    OUTPUT_NAME "AxiomTelemetryWorkbench"
)
set_source_files_properties(qml/Main.qml PROPERTIES QT_RESOURCE_ALIAS Main.qml)
set_source_files_properties(qml/components/MetricCard.qml PROPERTIES QT_RESOURCE_ALIAS components/MetricCard.qml)
set_source_files_properties(qml/components/TrendChart.qml PROPERTIES QT_RESOURCE_ALIAS components/TrendChart.qml)

qt_add_qml_module(AutomotiveTelemetryConsole
    URI AutomotiveTelemetry
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/MetricCard.qml
        qml/components/TrendChart.qml
        qml/components/StartupSplash.qml
)

target_include_directories(AutomotiveTelemetryConsole PRIVATE src)
target_link_libraries(AutomotiveTelemetryConsole PRIVATE Qt6::Quick Qt6::Network Qt6::SerialPort)
configure_project_target(AutomotiveTelemetryConsole)

include(CTest)
if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)

    qt_add_executable(PacketParserTests tests/tst_packetparser.cpp src/PacketParser.cpp src/PacketParser.h src/TelemetryFrame.h)
    target_include_directories(PacketParserTests PRIVATE src)
    target_link_libraries(PacketParserTests PRIVATE Qt6::Test)
    configure_project_target(PacketParserTests)
    add_test(NAME PacketParserTests COMMAND PacketParserTests)

    qt_add_executable(TcpIntegrationTests tests/tst_tcpintegration.cpp src/PacketParser.cpp src/PacketParser.h src/TelemetryFrame.h src/TelemetryWorker.cpp src/TelemetryWorker.h)
    target_include_directories(TcpIntegrationTests PRIVATE src)
    target_link_libraries(TcpIntegrationTests PRIVATE Qt6::Test Qt6::Network Qt6::SerialPort)
    configure_project_target(TcpIntegrationTests)
    add_test(NAME TcpIntegrationTests COMMAND TcpIntegrationTests)

    qt_add_executable(UdpIntegrationTests tests/tst_udpintegration.cpp src/PacketParser.cpp src/PacketParser.h src/TelemetryFrame.h src/TelemetryWorker.cpp src/TelemetryWorker.h)
    target_include_directories(UdpIntegrationTests PRIVATE src)
    target_link_libraries(UdpIntegrationTests PRIVATE Qt6::Test Qt6::Network Qt6::SerialPort)
    configure_project_target(UdpIntegrationTests)
    add_test(NAME UdpIntegrationTests COMMAND UdpIntegrationTests)

    qt_add_executable(CsvLoggerTests tests/tst_csvlogger.cpp src/CsvLogger.cpp src/CsvLogger.h src/TelemetryFrame.h)
    target_include_directories(CsvLoggerTests PRIVATE src)
    target_link_libraries(CsvLoggerTests PRIVATE Qt6::Test)
    configure_project_target(CsvLoggerTests)
    add_test(NAME CsvLoggerTests COMMAND CsvLoggerTests)

    qt_add_executable(FunctionalSmokeTests
        tests/tst_functionalsmoke.cpp
        src/CsvLogger.cpp src/CsvLogger.h
        src/PacketParser.cpp src/PacketParser.h
        src/TelemetryFrame.h
        src/TelemetryWorker.cpp src/TelemetryWorker.h
        src/TelemetryController.cpp src/TelemetryController.h)
    target_include_directories(FunctionalSmokeTests PRIVATE src)
    target_link_libraries(FunctionalSmokeTests PRIVATE Qt6::Test Qt6::Network Qt6::SerialPort)
    configure_project_target(FunctionalSmokeTests)
    add_test(NAME FunctionalSmokeTests COMMAND FunctionalSmokeTests)
endif()

if(BUILD_BENCHMARKS)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    qt_add_executable(ParserBenchmark benchmarks/tst_parserbenchmark.cpp src/PacketParser.cpp src/PacketParser.h src/TelemetryFrame.h)
    target_include_directories(ParserBenchmark PRIVATE src)
    target_link_libraries(ParserBenchmark PRIVATE Qt6::Test)
    configure_project_target(ParserBenchmark)
endif()

install(TARGETS AutomotiveTelemetryConsole BUNDLE DESTINATION . RUNTIME DESTINATION bin)
