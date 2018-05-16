#include <cassert>
#include "TcpConnectionPool.h"
#include "ConnectionPool.h"


using namespace emc;
int main(int argc, char *argv[]) {
    std::cout << "Creating connections..." << std::endl;
    std::shared_ptr<TCPConnectionFactory> connection_factory(new TCPConnectionFactory("10.249.248.192", 9069));
    std::shared_ptr<ConnectionPool<TCPConnection> > pool(new ConnectionPool<TCPConnection>(2, connection_factory));   
    ConnectionPoolStats stats = pool->get_stats();
    assert(stats.pool_size==2);
}
