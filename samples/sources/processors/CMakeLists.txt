# Build processors modules

include( processor.cmake )

add_processor_module( "atr" )
add_processor_module( "ema" )
add_processor_module( "roc" )
add_processor_module( "rsi" )
add_processor_module( "fftnoise" )

