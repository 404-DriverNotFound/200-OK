#pragma once
#include <sys/select.h>
#include <sys/types.h>

typedef u_int8_t	uint8_t;
typedef u_int16_t	uint16_t;
typedef u_int32_t	uint32_t;
typedef u_int64_t	uint64_t;

struct FdSet : public fd_set
{
	public:
				FdSet();
	FdSet&		operator=(FdSet&);
	virtual		~FdSet();
	bool		get(uint64_t fd) const;		// == is_set
	void		set(uint64_t fd);
	void		del(uint64_t fd);
	void		zero();
	void		print_bit() const;
};
