/*
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	$Id: br_private.h,v 1.1.1.1 2006-01-16 08:12:14 jeff Exp $
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#ifndef _BR_PRIVATE_H
#define _BR_PRIVATE_H

#include <linux/netdevice.h>
#include <linux/miscdevice.h>
#include <linux/if_bridge.h>
#include "br_private_timer.h"

#include "br_mfd.h"

#if 1
#define DEBUGP(format, args...)
#else
#define DEBUGP printk
#endif


#define BR_HASH_BITS 8
#define BR_HASH_SIZE (1 << BR_HASH_BITS)

#define BR_HOLD_TIME (1*HZ)

#define MAC_ADDR_LEN	  6
#define MATCH_PROTO       1
#define MATCH_SRC_MAC  1<<1
#define MATCH_DST_MAC  1<<2
#define MATCH_SPORT    1<<3
#define MATCH_DPORT    1<<4


typedef struct bridge_id bridge_id;
typedef struct mac_addr mac_addr;
typedef __u16 port_id;

struct bridge_id
{
	unsigned char	prio[2];
	unsigned char	addr[6];
};

struct mac_addr
{
	unsigned char	addr[6];
	unsigned char	pad[2];
};

struct net_bridge_fdb_entry
{
	struct net_bridge_fdb_entry	*next_hash;
	struct net_bridge_fdb_entry	**pprev_hash;
	atomic_t			use_count;
	mac_addr			addr;
	struct net_bridge_port		*dst;
	unsigned long			ageing_timer;
	unsigned			is_local:1;
	unsigned			is_static:1;
};


struct bridge_fw_db_entry
{
	struct bridge_fw_db_entry	*next;
	atomic_t			use_count;
        char                            sport[10];
        char                            dport[10];
	unsigned char			src_mac_addr[6];
        unsigned char		        dst_mac_addr[6];
        unsigned short                  eth_proto;
        unsigned char                   access_type;
        unsigned char                   match_check;
        unsigned int                    frames_matched;
};


struct net_bridge_port
{
	struct net_bridge_port		*next;
	struct net_bridge		*br;
	struct net_device		*dev;
	int				port_no;

	/* STP */
	port_id				port_id;
	int				state;
	int				path_cost;
	bridge_id			designated_root;
	int				designated_cost;
	bridge_id			designated_bridge;
	port_id				designated_port;
	unsigned			topology_change_ack:1;
	unsigned			config_pending:1;
	int				priority;

	struct br_timer			forward_delay_timer;
	struct br_timer			hold_timer;
	struct br_timer			message_age_timer;

};

struct net_bridge
{
	struct net_bridge		*next;
	rwlock_t			lock;
	struct net_bridge_port		*port_list;
	struct net_device		dev;
	struct net_device_stats		statistics;
	rwlock_t			hash_lock;
	struct net_bridge_fdb_entry	*hash[BR_HASH_SIZE];
        rwlock_t			filter_lock;
	struct bridge_fw_db_entry       *blacklist;
	struct timer_list		tick;
#ifdef CONFIG_BRIDGED_MULTICAST
	rwlock_t			mfd_hash_lock;
	struct net_bridge_mfd_entry	*mfd_hash[BR_HASH_SIZE];
	int 				mc_forwarding;
#ifdef CONFIG_BRIDGE_CONF_MODE
	int                             conf_mode;
	int                             isQuerier;
	struct timer_list               general_query_timer;
	struct timer_list               other_querier_present_timer;
#endif
#endif

	/* STP */
	bridge_id			designated_root;
	int				root_path_cost;
	int				root_port;
	int				max_age;
	int				hello_time;
	int				forward_delay;
	bridge_id			bridge_id;
	int				bridge_max_age;
	int				bridge_hello_time;
	int				bridge_forward_delay;
	unsigned			stp_enabled:1;
	unsigned			topology_change:1;
	unsigned			topology_change_detected:1;
        unsigned                        br_filter_active:1;

	struct br_timer			hello_timer;
	struct br_timer			tcn_timer;
	struct br_timer			topology_change_timer;
	struct br_timer			gc_timer;

	int				ageing_time;
	int				gc_interval;
};

extern struct notifier_block br_device_notifier;
extern unsigned char bridge_ula[6];

/* br.c */
extern void br_dec_use_count(void);
extern void br_inc_use_count(void);

/* br_device.c */
extern void br_dev_setup(struct net_device *dev);

/* br_fdb.c */
extern void br_fdb_changeaddr(struct net_bridge_port *p,
		       unsigned char *newaddr);
extern void br_fdb_cleanup(struct net_bridge *br);
extern void br_fdb_delete_by_port(struct net_bridge *br,
			   struct net_bridge_port *p);
extern struct net_bridge_fdb_entry *br_fdb_get(struct net_bridge *br,
					unsigned char *addr);
extern void br_fdb_put(struct net_bridge_fdb_entry *ent);
extern int  br_fdb_get_entries(struct net_bridge *br,
			unsigned char *_buf,
			int maxnum,
			int offset);
extern void br_fdb_insert(struct net_bridge *br,
		   struct net_bridge_port *source,
		   unsigned char *addr,
		   int is_local);

/* br_forward.c */
extern void br_deliver(struct net_bridge_port *to,
		struct sk_buff *skb);
extern void br_forward(struct net_bridge_port *to,
		struct sk_buff *skb);
extern void br_flood_deliver(struct net_bridge *br,
		      struct sk_buff *skb,
		      int clone);
extern void br_flood_forward(struct net_bridge *br,
		      struct sk_buff *skb,
		      int clone);

/* br_if.c */
extern struct net_bridge **__find_br(char *name);
extern int br_add_bridge(char *name);
extern int br_del_bridge(char *name);
extern int br_add_if(struct net_bridge *br,
	      struct net_device *dev);
extern int br_del_if(struct net_bridge *br,
	      struct net_device *dev);
extern int br_get_bridge_ifindices(int *indices,
			    int num);
extern void br_get_port_ifindices(struct net_bridge *br,
			   int *ifindices);

/* br_input.c */
extern void br_handle_frame(struct sk_buff *skb);
extern void br_pass_frame_up(struct net_bridge *br, struct sk_buff *skb);
/* br_filter.c */
int br_add_filter_entry(struct net_bridge *br, unsigned char *_entry);
int br_del_filter_entry(struct net_bridge *br, unsigned char *_entry);
int br_flush_filter(struct net_bridge *br);
int br_show_filter_entries(struct net_bridge *br, unsigned char *_buf, int maxnum);

/* br_ioctl.c */
extern void br_call_ioctl_atomic(void (*fn)(void));
extern int br_ioctl(struct net_bridge *br,
	     unsigned int cmd,
	     unsigned long arg0,
	     unsigned long arg1,
	     unsigned long arg2);
extern int br_ioctl_deviceless_stub(unsigned long arg);

/* br_stp.c */
extern int br_is_root_bridge(struct net_bridge *br);
extern struct net_bridge_port *br_get_port(struct net_bridge *br,
				    int port_no);
extern void br_init_port(struct net_bridge_port *p);
extern port_id br_make_port_id(struct net_bridge_port *p);
extern void br_become_designated_port(struct net_bridge_port *p);

/* br_stp_if.c */
extern void br_stp_enable_bridge(struct net_bridge *br);
extern void br_stp_disable_bridge(struct net_bridge *br);
extern void br_stp_enable_port(struct net_bridge_port *p);
extern void br_stp_disable_port(struct net_bridge_port *p);
extern void br_stp_recalculate_bridge_id(struct net_bridge *br);
extern void br_stp_set_bridge_priority(struct net_bridge *br,
				int newprio);
extern void br_stp_set_port_priority(struct net_bridge_port *p,
			      int newprio);
extern void br_stp_set_path_cost(struct net_bridge_port *p,
			  int path_cost);

/* br_stp_bpdu.c */
extern void br_stp_handle_bpdu(struct sk_buff *skb);

#endif
