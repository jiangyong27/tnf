#include "net_session.h"


int CSessionMgr::Set(uint32_t msg_seq, CBaseSession* timer_info, uint32_t gap_ms)
{
    struct timeval now;
    gettimeofday(&now, NULL);

	timer_info->_msg_seq = msg_seq;
    timer_info->_last_time = now;
	timer_info->_gap = gap_ms;
    if (_mp_timer_info.find(msg_seq) == _mp_timer_info.end()) {
	    _mp_timer_info[msg_seq] = timer_info;
    } else {
        return -1;
    }
	return 0;
}

int CSessionMgr::Get(uint32_t msg_seq, CBaseSession** timer_info)
{
    HHMap<uint32_t, CBaseSession*>::iterator it = _mp_timer_info.find(msg_seq);
	if (it == _mp_timer_info.end())
	{
		*timer_info = NULL;
		return -1;
	}
	*timer_info = it->second;
	_mp_timer_info.erase(it);
	return 0;
}

int CSessionMgr::Exist(uint32_t msg_seq)
{
	if (_mp_timer_info.find(msg_seq) != _mp_timer_info.end()) {
		return 0;
	}
	else {
		return 1;
	}
}

void CSessionMgr::CheckExpire()
{
	struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    HHMap<uint32_t, CBaseSession*>::iterator it = _mp_timer_info.begin();
	while (it != _mp_timer_info.end())
	{
		CBaseSession *timer_info  = it->second;
		if (TIME_DIFF_MS(cur_time, timer_info->_last_time) > timer_info->_gap)
		{
		    timer_info->OnExpire();
			if (timer_info->GetDelete() == true)
			{
				delete timer_info;
				timer_info = NULL;
			}
			_mp_timer_info.erase(it++);

            /*
			if(timer_info != NULL)
			{
				if(timer_info->GetDelete() == false)
				{
					timer_info->SetDelete(true);
				}
			}
            */
		}
		else
		{
			it++;
		}
	}
}
