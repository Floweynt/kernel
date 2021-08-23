#ifndef __KERNEL_SCHEDULER_TYPES__
#define __KERNEL_SCHEDULER_TYPES__

#define

struct process_priority
{
	uint64_t last_scheduled_ticked;
	uint8_t priority;
};

class scheduler
{

};

#endif
