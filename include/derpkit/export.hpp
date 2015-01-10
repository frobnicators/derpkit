#ifndef DERPKIT_EXPORT_HPP
#define DERPKIT_EXPORT_HPP

//TODO: Styr upp den här skitet, det här kommer inte funka vid användning

#define DERPKIT_EXPORT __attribute__((visibility("default")))
#ifdef ENABLE_DEBUG
#define DERPKIT_DEBUG_EXPORT __attribute__((visibility("default")))
#endif

#endif /* DERPKIT_EXPORT_HPP */
