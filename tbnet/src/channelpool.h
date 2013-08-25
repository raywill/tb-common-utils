/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */

#ifndef TBNET_CHANNEL_POOL_H_
#define TBNET_CHANNEL_POOL_H_

#define CHANNEL_CLUSTER_SIZE 25
namespace tbnet {

// channel pool基本等价于session pool，channel等价于session，多个packet可以属于一个channel
class ChannelPool {

public:
    /*
     * 构造函数
     */
    ChannelPool();

    /*
     * 析构函数
     */
    ~ChannelPool();

    /*
     * 得到一个新的channel
     *
     * @return 一个Channel
     */
    Channel *allocChannel();

    /*
     * 释放一个channel
     *
     * @param channel: 要释放的channel
     * @return
     */
    bool freeChannel(Channel *channel);
    bool appendChannel(Channel *channel);

    /*
     * 查找一下channel
     *
     * @param id: channel id
     * @return Channel
     */
    Channel* offerChannel(uint32_t id);

    /*
     * 从useList中找出超时的channel的list,并把hashmap中对应的删除
     *
     * @param now: 当前时间
     */
    Channel* getTimeoutList(int64_t now);

    /*
     * 把addList的链表加入到freeList中
     *
     * @param addList被加的list
     */
    bool appendFreeList(Channel *addList);

    /*
     * 被用链表的长度
     */
    int getUseListCount() {
        return _useMap.size();
    }

    void setExpireTime(Channel *channel, int64_t now); 

private:
    __gnu_cxx::hash_map<uint32_t, Channel*> _useMap; // 使用的map
    std::list<Channel*> _clusterList;                // cluster list
    tbsys::CThreadMutex _mutex;

    Channel *_freeListHead;             // 空的链表
    Channel *_freeListTail;
    Channel *_useListHead;              // 被使用的链表
    Channel *_useListTail;
    int _maxUseCount;                   // 被用链表的长度

    static atomic_t _globalChannelId;   // 生成统一的id
    static atomic_t _globalTotalCount;
};

}

#endif /*CHANNEL_POOL_H_*/
