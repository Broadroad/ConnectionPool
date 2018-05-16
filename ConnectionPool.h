#ifndef CONNECTIONPOOL_H_
#define CONNECTIONPOOL_H_
#include <deque>
#include <set>
#include <iostream>
#include <exception>
#include <memory>
#include <mutex>

namespace emc {

struct ConnectionUnavaliable : std::exception { 
	char const* what() const throw() {
		return "Unable to allocate connection";
	}; 
};

class Connection {
public:
    Connection(){};
    virtual ~Connection(){};
};

class ConnectionFactory {
public:
    virtual std::shared_ptr<Connection> create() = 0;
};

struct ConnectionPoolStats {
    size_t pool_size;
    size_t borrowed_size;
};

template<class T>
class ConnectionPool {
public:
    ConnectionPool(size_t pool_size, std::shared_ptr<ConnectionFactory> factory) {
        pool_size_ = pool_size;
        factory_ = factory;

        // Generate the Pool
        while (this->pool_.size() < this->pool_size_) {
            this->pool_.push_back(this->factory_->create());
        }
    };

    ~ConnectionPool() {

    };

    std::shared_ptr<T> borrow() {
        std::lock_guard<std::mutex> lock(io_mutex_);

        // Check if there exist a free connection
        if (this->pool_.size() == 0) {
            
            // If there is some crashed connections, then don't need to throw exception
            for (auto it = this->borrowed_.begin(); it != borrowed_.end(); ++it) {
                if ((*it).unique()) {
                    try {
                        // Create a new connection and replace it
                        std::shared_ptr<Connection> conn = factory_->create();
                        borrowed_.erase(it);
                        borrowed_.insert(conn);

                        return std::dynamic_pointer_cast<T>(conn);
                    } catch (std::exception& e) {
                        throw e;
                    }
                }
            }

            throw ConnectionUnavaliable();
        }

        std::shared_ptr<Connection> conn = pool_.front();
        pool_.pop_front();

        borrowed_.insert(conn);
        return std::dynamic_pointer_cast<T>(conn);
    }

    void unborrow(std::shared_ptr<T> conn) {
        std::lock_guard<std::mutex> lock(io_mutex_);

        pool_.push_back(std::dynamic_pointer_cast<Connection>(conn));

        borrowed_.erase(conn);
    }

    ConnectionPoolStats get_stats() {
        // Get Lock
        std::lock_guard<std::mutex> lock(io_mutex_);

        // Get stats
        ConnectionPoolStats stats;
        stats.pool_size = pool_.size();
        stats.borrowed_size = borrowed_.size();

        return stats;
    };


protected:
    std::shared_ptr<ConnectionFactory> factory_;
    size_t pool_size_;
    std::deque<std::shared_ptr<Connection> > pool_;
    std::set<std::shared_ptr<Connection> > borrowed_;
    std::mutex io_mutex_;
};


}


#endif /* CONNECTIONPOOL_H_ */
