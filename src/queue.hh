#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <stdexcept>
#include <cassert>

namespace krc {

class queue_closed : public std::logic_error
{
public:
  using std::logic_error::logic_error;
};

template <typename T>
class queue
{
public:
  explicit queue(size_t max_size);

  size_t size() const;

  bool empty() const;

  size_t max_size() const;

  void push(T && item);

  std::optional<T> pop();

  void close();

private:
  typedef std::unique_lock<std::mutex> lock_t;

  bool closed() const;

  bool not_full() const;

  bool not_empty() const;

  size_t d_max_size;
  std::queue<T> d_base;

  mutable std::mutex d_mutex;
  std::condition_variable d_not_full;
  std::condition_variable d_not_empty;
  bool d_closed{false};
};

template <typename T>
queue<T>::queue(size_t max_size)
  : d_max_size(max_size)
{
  assert(d_max_size > 0);
}

template <typename T>
size_t queue<T>::max_size() const
{
  return d_max_size;
}

template <typename T>
void queue<T>::push(T && item)
{
  lock_t l(d_mutex);
  d_not_full.wait(l, [=]{ return closed() || this->not_full(); });

  if(closed())
    throw queue_closed("push on a closed queue");

  d_base.emplace(std::forward<T>(item));

  l.unlock();
  d_not_empty.notify_one();
}

template <typename T>
std::optional<T> queue<T>::pop()
{
  lock_t l(d_mutex);

  d_not_empty.wait(l, [=]{ return closed() || this->not_empty(); });

  if(!not_empty())
    return std::optional<T>();

  auto item = d_base.front();
  d_base.pop();

  l.unlock();
  d_not_full.notify_one();

  return item;
}

template <typename T>
bool queue<T>::not_full() const
{
  return d_base.size() < max_size();
}

template <typename T>
bool queue<T>::not_empty() const
{
  return !d_base.empty();
}

template <typename T>
void queue<T>::close()
{
  lock_t l(d_mutex);
  d_closed = true;
  l.unlock();

  d_not_full.notify_all();
  d_not_empty.notify_all();
}

template <typename T>
bool queue<T>::closed() const
{
  return d_closed;
}

template <typename T>
size_t queue<T>::size() const
{
  lock_t l(d_mutex);
  return d_base.size();
}

template <typename T>
bool queue<T>::empty() const
{
  lock_t l(d_mutex);
  return d_base.empty();
}

}
