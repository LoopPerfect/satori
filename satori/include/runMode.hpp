#ifndef SATORI_RUN_MODE_HPP
#define SATORI_RUN_MODE_HPP

namespace satori {

/**
 *  C++ wrapper for uv_run_mode
 */
enum class RunMode { Default, Once, NoWait };
}

#endif
