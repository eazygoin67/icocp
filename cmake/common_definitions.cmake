set(ICOCP_GLOBAL_DEFINITIONS ${ICOCP_GLOBAL_DEFINITIONS}
    _UNICODE
    UNICODE
    _HAS_EXCEPTIONS=0
    WIN32_LEAN_AND_MEAN
)

set(ICOCP_GLOBAL_OPTIONS ${ICOCP_GLOBAL_OPTIONS}
    /MP 
    /EHsc
    /Zc:static_assert-
    /bigobj
)

set(ICOCP_GLOBAL_WARNING_LEVELS ${ICOCP_GLOBAL_WARNING_LEVELS}
    /W4 
    /WX 
    /wd4100 
    /wd4127 
    /wd4201 
    /wd4706 
    /wd5054
    /wd4505 
	/wd4996
)
  
set(ICOCP_GLOBAL_LINK_OPTIONS ${ICOCP_GLOBAL_LINK_OPTIONS} 
    "$<IF:$<OR:$<CONFIG:${ICOCP_GLOBAL_CONFIG_DEBUG}>,$<CONFIG:${ICOCP_GLOBAL_CONFIG_TESTING}>>,/INCREMENTAL,/INCREMENTAL:NO>"
)