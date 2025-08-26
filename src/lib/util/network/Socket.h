/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_LIB_UTIL_NETWORK_SOCKET_H
#define HHUOS_LIB_UTIL_NETWORK_SOCKET_H

#include <stdint.h>

#include "io/file/File.h"
#include "time/Timestamp.h"

namespace Util {
template <typename T> class Array;

namespace Network {
class NetworkAddress;
class Datagram;

namespace Ip4 {
class Ip4Route;
class Ip4SubnetAddress;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Util::Network {

/// Represents a network socket, which is used to send and receive data over a network.
/// A socket can be bound to a specific network address and can send and receive
/// datagrams of the type given at its construction (e.g. UDP or ICMP).
/// The socket itself lives in the kernel and is represented by a file descriptor.
/// This class wraps the file descriptor and provides methods to interact with the socket.
/// It also provides methods to manage the IPv4 addresses and routes of a network interface,
/// which only work if the socket is of type `ETHERNET` and bound to the MAC address of the interface.
///
/// ## Example
/// ```c++
/// // Create a new UDP socket
/// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
///
/// // Bind the socket to a specific address
/// if (!socket.bind(Util::Network::Ip4::Ip4PortAddress("10.0.2.15:1797"))) {
///     Util::System::out << "Failed to bind socket!"
///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
///     return;
/// }
///
/// // Set a timeout for receiving data
/// socket.setTimeout(Util::Time::Timestamp::ofSeconds(5));
///
/// // Try to receive a datagram
/// auto receivedDatagram = Util::Network::Udp::UdpDatagram();
/// if (!socket.receive(receivedDatagram)) {
///     Util::System::out << "Failed to receive datagram!"
///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
///     return;
/// }
/// ```
class Socket {

public:
    /// Different types of sockets that can be created.
    enum Type {
        /// Ethernet socket, used for low-level network communication.
        /// This can be used for low-level network communication using raw Ethernet frames,
        /// but is not commonly used in applications.
        ETHERNET,
        /// Socket for Internet Protocol version 4 (IPv4) communication.
        IP4,
        /// Socket for Internet Protocol version 6 (IPv6) communication (not supported yet).
        IP6,
        /// Socket for Internet Control Message Protocol (ICMP) communication, typically used for ping requests.
        ICMP,
        /// Socket for User Datagram Protocol (UDP) communication, used for sending and receiving datagrams.
        /// This is the most common type of socket used in network applications.
        UDP,
        /// Socket for Transmission Control Protocol (TCP) communication (not supported yet).
        TCP
    };

    /// Requests that can be issued to the socket via file control operations.
    /// This is typically not done directly by applications, since all requests are wrapped in methods of this class.
    enum Request {
        /// Set the timeout for receiving data on the socket.
        /// A blocking read request will return with no data read after the timeout expires.
        SET_TIMEOUT,
        /// Bin the socket to a specific network address.
        BIND,
        /// Get the network address the socket is bound to.
        GET_LOCAL_ADDRESS,
        /// Get all IPv4 addresses that are assigned to the interface this socket is bound to.
        /// This only works if the socket is of type `ETHERNET` and bound to a MAC address.
        GET_IP4_ADDRESSES,
        /// Remove an IPv4 address from the interface this socket is bound to.
        /// This only works if the socket is of type `ETHERNET` and bound to a MAC address.
        REMOVE_IP4_ADDRESS,
        /// Add an IPv4 address to the interface this socket is bound to.
        /// This only works if the socket is of type `ETHERNET` and bound to a MAC address.
        ADD_IP4_ADDRESS,
        /// Get all IPv4 routes that deliver packets via the interface this socket is bound to.
        /// This only works if the socket is of type `IP4` and bound to an IPv4 address.
        GET_ROUTES,
        /// Remove an IPv4 route from the interface this socket is bound to.
        /// This only works if the socket is of type `IP4` and bound to a IPv4 address.
        REMOVE_ROUTE,
        /// Add an IPv4 route to the interface this socket is bound to.
        /// This only works if the socket is of type `IP4` and bound to a IPv4 address.
        ADD_ROUTE
    };

    /// Sockets are not copyable, so the copy constructor is deleted.
    Socket(const Socket &other) = delete;

    /// Sockets are not copyable, so the assignment operator is deleted.
    Socket &operator=(const Socket &other) = delete;

    /// Close the socket and free the file descriptor.
    ~Socket();

    /// Create a new socket of the given type.
    /// The socket is created in the kernel via a system call and can be controlled via the returned instance.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new UDP socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
    /// ```
    static Socket createSocket(Type type);

    /// Set the timeout for receiving data on the socket.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new UDP socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
    ///
    /// // Set a timeout for receiving data
    /// socket.setTimeout(Util::Time::Timestamp::ofSeconds(5));
    /// ```
    void setTimeout(Time::Timestamp timeout) const;

    /// Bind the socket to a specific network address.
    /// This will be the source address for all datagrams sent from this socket
    /// and all datagrams sento to this address will be received by this socket.
    /// The address must fit the type that the socket was created with (e.g. `Ip4::Ip4PortAddress` for a UDP socket).
    /// The return value indicates whether the binding was successful or not.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new UDP socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
    ///
    /// // Bind the socket to a specific address
    /// if (!socket.bind(Util::Network::Ip4::Ip4PortAddress("10.0.2.15:1797"))) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    [[nodiscard]] bool bind(const NetworkAddress &address) const;

    /// Get the local address of the socket, which is the address it is bound to.
    /// The address is not returned, but written to the given `NetworkAddress` object.
    /// This operation fails, if the socket is not bound to an address.
    /// The return value indicates whether the address could be retrieved successfully.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new UDP socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
    ///
    /// // Bind the socket to a specific address
    /// if (!socket.bind(Util::Network::Ip4::Ip4PortAddress("10.0.2.15:1797"))) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// // Get the local address of the socket
    /// auto localAddress = Util::Network::NetworkAddress();
    /// if (!socket.getLocalAddress(localAddress)) {
    ///     Util::System::out << "Failed to get local address!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// const auto addressString = localAddress.toString(); // "10.0.2.15:1797"
    /// ```
    [[nodiscard]] bool getLocalAddress(NetworkAddress &address) const;

    /// Send a datagram via the socket.
    /// The datagram must be of the type that the socket was created with (e.g `Udp::UdpDatagram` for a UDP socket).
    /// The return value indicates whether the datagram was sent successfully.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new UDP socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
    ///
    /// // Bind the socket to a specific address
    /// if (!socket.bind(Util::Network::Ip4::Ip4PortAddress("10.0.2.15:1797"))) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// // Create a datagram to send (Payload: "Hello, World!", Destination: "10.0.2.2:1856")
    /// const auto destinationAddress = Util::Network::Ip4::Ip4PortAddress("10.0.2.2:1856");
    /// const auto datagram = Util::Network::Udp::UdpDatagram(
    ///     reinterpret_cast<const uint8_t*>("Hello, World!"), 13, destinationAddress);
    ///
    /// // Send the datagram via the socket
    /// if (!socket.send(datagram)) {
    ///     Util::System::out << "Failed to send datagram!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    [[nodiscard]] bool send(const Datagram &datagram) const;

    /// Receive a datagram from the socket.
    /// The received data is not returned, but written to the given `Datagram` object.
    /// The datagram must be of the type that the socket was created with (e.g `Udp::UdpDatagram` for a UDP socket).
    /// The return value indicates whether a datagram was received successfully.
    /// This method will block until either a datagram is received or the timeout set by `setTimeout()` expires.
    /// To change this behavior, so that this method returns immediately if no datagram is available,
    /// use `setAccessMode()` to set the socket to non-blocking mode.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new UDP socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
    ///
    /// // Bind the socket to a specific address
    /// if (!socket.bind(Util::Network::Ip4::Ip4PortAddress("10.0.2.15:1797"))) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// // Set a timeout for receiving data
    /// socket.setTimeout(Util::Time::Timestamp::ofSeconds(5));
    ///
    /// // Try to receive a datagram
    /// auto receivedDatagram = Util::Network::Udp::UdpDatagram();
    /// if (!socket.receive(receivedDatagram)) {
    ///     Util::System::out << "Failed to receive datagram!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    [[nodiscard]] bool receive(Datagram &datagram) const;

    /// Get all IPv4 addresses that are assigned to the interface this socket is bound to.
    /// This only works if the socket is of type `ETHERNET` and bound to a MAC address.
    /// The addresses are returned as an array of `Ip4::Ip4SubnetAddress` objects.
    /// If the call fails or no IPv4 addresses are found, an empty array is returned.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new Ethernet socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
    ///
    /// // Read MAC address of device "eth0" from file system
    /// auto macFile = Util::Io::FileInputStream("/device/eth0/mac");
    /// bool endOfFile = false;
    /// auto macAddress = Util::Network::MacAddress(macFile.readLine(endOfFile));
    ///
    /// // Bind the socket to the read MAC address
    /// if (!socket.bind(macAddress)) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// // Query the IPv4 addresses assigned to the interface and print them
    /// const auto ipAddresses = socket.getIp4Addresses();
    /// for (const auto &address : ipAddresses) {
    ///     Util::System::out << "IPv4 Address: " << address.toString() << Util::Io::PrintStream::endl;
    /// }
    ///
    /// Util::System::out << Util::Io::PrintStream::flush;
    /// ```
    [[nodiscard]] Array<Ip4::Ip4SubnetAddress> getIp4Addresses() const;

    /// Remove an IPv4 address from the interface this socket is bound to.
    /// This only works if the socket is of type `ETHERNET` and bound to a MAC address.
    /// The given IPv4 address must be of type `Ip4::Ip4SubnetAddress`.
    /// The return value indicates whether the address was removed successfully.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new Ethernet socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
    ///
    /// // Read MAC address of device "eth0" from file system
    /// auto macFile = Util::Io::FileInputStream("/device/eth0/mac");
    /// bool endOfFile = false;
    /// auto macAddress = Util::Network::MacAddress(macFile.readLine(endOfFile));
    ///
    /// // Bind the socket to the read MAC address
    /// if (!socket.bind(macAddress)) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// /// Remove an IPv4 address from the interface
    /// const auto addressToRemove = Util::Network::Ip4::Ip4SubnetAddress("10.0.2.15/24");
    /// if (!socket.removeIp4Address(addressToRemove)) {
    ///     Util::System::out << "Failed to remove IPv4 address!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    [[nodiscard]] bool removeIp4Address(const Ip4::Ip4SubnetAddress &address) const;

    /// Add an IPv4 address to the interface this socket is bound to.
    /// This only works if the socket is of type `ETHERNET` and bound to a MAC address.
    /// The given IPv4 address must be of type `Ip4::Ip4SubnetAddress`.
    /// The return value indicates whether the address was added successfully.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new Ethernet socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
    ///
    /// // Read MAC address of device "eth0" from file system
    /// auto macFile = Util::Io::FileInputStream("/device/eth0/mac");
    /// bool endOfFile = false;
    /// auto macAddress = Util::Network::MacAddress(macFile.readLine(endOfFile));
    ///
    /// // Bind the socket to the read MAC address
    /// if (!socket.bind(macAddress)) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// /// Add an IPv4 address to the interface
    /// const auto addressToAdd = Util::Network::Ip4::Ip4SubnetAddress("10.0.2.15/24");
    /// if (!socket.addIp4Address(addressToAdd)) {
    ///     Util::System::out << "Failed to add IPv4 address!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    [[nodiscard]] bool addIp4Address(const Ip4::Ip4SubnetAddress &address) const;

    /// Get all IPv4 routes that deliver packets via the interface this socket is bound to.
    /// This only works if the socket is of type `IP4` and bound to an IPv4 address.
    /// The routes are returned as an array of `Ip4::Ip4Route` objects.
    /// If the call fails or no routes are found, an empty array is returned.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new IPv4 socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
    ///
    /// // Bind the socket to a specific IPv4 address
    /// if (!socket.bind(Util::Network::Ip4::Ip4Address("10.0.2.15"))) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// // Query the IPv4 routes and print them
    /// const auto routes = socket.getRoutes();
    /// for (const auto &route : routes) {
    ///     Util::System::out << "Route: " << route.getTargetAddress().toString()
    ///         << " via " << (route.hasNextHop() ? route.getNextHop().toString() : "N/A")
    ///         << " on device " << route.getDeviceIdentifier()
    ///         << Util::Io::PrintStream::endl;
    /// }
    ///
    /// Util::System::out << Util::Io::PrintStream::flush;
    /// ```
    [[nodiscard]] Array<Ip4::Ip4Route> getRoutes() const;

    /// Remove a specific IPv4 route from the interface this socket is bound to.
    /// This only works if the socket is of type `IP4` and bound to an IPv4 address.
    /// The return value indicates whether the route was removed successfully.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new IPv4 socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
    ///
    /// // Bind the socket to a specific IPv4 address
    /// if (!socket.bind(Util::Network::Ip4::Ip4Address("10.0.2.15"))) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// // Remove a specific IPv4 route. In this case, the route delivers packets into "10.0.2.0/24" subnet
    /// // via the interface "eth0" with "10.0.2.15" as the source address.
    /// const auto routeToRemove = Util::Network::Ip4::Ip4Route(
    ///         Util::Network::Ip4::Ip4SubnetAddress("10.0.2.15/24"), "eth0");
    ///
    /// if (!socket.removeRoute(routeToRemove)) {
    ///     Util::System::out << "Failed to remove route!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    [[nodiscard]] bool removeRoute(const Ip4::Ip4Route &route) const;

    /// Add a new IPv4 route to the interface this socket is bound to.
    /// This only works if the socket is of type `IP4` and bound to an IPv4 address.
    /// The return value indicates whether the route was added successfully.
    ///
    /// ### Example
    /// ```c++
    /// // Create a new IPv4 socket
    /// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
    ///
    /// // Bind the socket to a specific IPv4 address
    /// if (!socket.bind(Util::Network::Ip4::Ip4Address("10.0.2.15"))) {
    ///     Util::System::out << "Failed to bind socket!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// // Add a new IPv4 route. In this case, the route delivers packets into "10.0.2.0/24" subnet
    /// // via the interface "eth0" with "10.0.2.15" as the source address.
    /// const auto routeToAdd = Util::Network::Ip4::Ip4Route(
    ///         Util::Network::Ip4::Ip4SubnetAddress("10.0.2.15/24"), "eth0");
    ///
    /// if (!socket.addRoute(routeToAdd)) {
    ///     Util::System::out << "Failed to add route!"
    ///         << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    [[nodiscard]] bool addRoute(const Ip4::Ip4Route &route) const;

    /// Set the access mode of the socket to block or non-blocking.
    /// This affects how the socket behaves when receiving data.
    /// If the access mode is set to non-blocking, the `receive()` method will
    /// return immediately if no data is available, instead of blocking until data is received.
    [[nodiscard]] bool setAccessMode(Io::File::AccessMode accessMode) const;

    /// Check if the socket is ready to read data.
    /// This method returns true if the socket has at least one datagram available to read.
    [[nodiscard]] bool isReadyToRead() const;

private:

    Socket(int32_t fileDescriptor, Type type);

    int32_t fileDescriptor;
    Type type;
};

}

#endif
