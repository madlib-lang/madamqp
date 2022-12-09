#include "gc.h"
#include "uv.h"
#include "amqpcpp.h"
#include "amqpcpp/libuv.h"
#include "event-loop.hpp"
#include "apply-pap.hpp"


// class MadChannel : AMQP::TcpChannel {
// private:
//   PAP_t *_onReadyCallback;
//   PAP_t *_onChannelFailedCallback;



// public:
//   MadChannel(AMQP::Connection *connection, PAP_t *onConnectionFailedCallback, PAP_t *onConnectedCallback) : AMQP::TcpChannel(connection) {
//     _onConnectedCallback = onConnectedCallback;
//     _onConnectionFailedCallback = onConnectionFailedCallback;
//   }

//   virtual ~MadChannel = default;
// }

class MadHandler : public AMQP::LibUvHandler {
private:
    PAP_t *_onConnectedCallback;
    PAP_t *_onConnectionFailedCallback;
    bool _hasConnected;

    /**
     *  Method that is called when a connection error occurs
     *  @param  connection
     *  @param  message
     */
    virtual void onError(AMQP::TcpConnection *connection, const char *message) override {
      if (!_hasConnected) {
        __applyPAP__(_onConnectionFailedCallback, 1, message);
      }
    }

    virtual void onReady(AMQP::TcpConnection *connection) override {
      _hasConnected = true;
      __applyPAP__(_onConnectedCallback, 1, connection);
    }

    virtual void onHeartbeat(AMQP::TcpConnection *connection) override {
      connection->heartbeat();
    }

public:
    /**
     *  Constructor
     *  @param  uv_loop
     */
    MadHandler(uv_loop_t *loop, PAP_t *onConnectionFailedCallback, PAP_t *onConnectedCallback) : AMQP::LibUvHandler(loop) {
      _onConnectedCallback = onConnectedCallback;
      _onConnectionFailedCallback = onConnectionFailedCallback;
      _hasConnected = false;
    }

    /**
     *  Destructor
     */
    virtual ~MadHandler() = default;
};

#ifdef __cplusplus
extern "C" {
#endif

void madamqp__connect(char *url, PAP_t *badCB, PAP_t *goodCB) {
  try {
    MadHandler *handler = new MadHandler(getLoop(), badCB, goodCB);
    new AMQP::TcpConnection(handler, AMQP::Address(url));
  } catch(char *err) {
    // TODO: copy message
    __applyPAP__(badCB, 1, err);
  } catch(std::runtime_error &err) {
    // TODO: copy message
    __applyPAP__(badCB, 1, err.what());
  } catch(...) {
    __applyPAP__(badCB, 1, "");
  }
}

void madamqp__disconnect(AMQP::TcpConnection *connection) {
  connection->close(true);
}


void madamqp__createChannel(AMQP::TcpConnection *connection, PAP_t *badCB, PAP_t *goodCB) {
  AMQP::TcpChannel *channel = new AMQP::TcpChannel(connection);
  bool *created = new bool;
  channel->onReady([channel, created, goodCB]() {
    *created = true;
    __applyPAP__(goodCB, 1, channel);
  });

  channel->onError([badCB, created](const char *message) {
    // TODO: copy message
    // Should not be called in case the channel was already successfully created!
    if (!*created) {
      __applyPAP__(badCB, 1, message);
    }
  });
}

void madamqp__consume(AMQP::TcpChannel *channel, char *queueName, PAP_t *onMessage) {
  channel->consume(queueName).onMessage([onMessage](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
    char *messageCopy = (char *) GC_MALLOC_ATOMIC(message.bodySize() + 1);
    memcpy(messageCopy, message.body(), message.bodySize());
    messageCopy[message.bodySize()] = '\0';

    __applyPAP__(onMessage, 2, messageCopy, (void*)deliveryTag);
  });
}

void madamqp__ack(AMQP::TcpChannel *channel, uint64_t deliveryTag) {
  channel->ack(deliveryTag);
}

void madamqp__nack(AMQP::TcpChannel *channel, uint64_t deliveryTag) {
  channel->reject(deliveryTag);
}

void madamqp__bindQueue(AMQP::TcpChannel *channel, char *exchange, char *queue, char *routingKey, PAP_t *badCB, PAP_t *goodCB) {
  channel->bindQueue(exchange, queue, routingKey)
    .onError([badCB](const char *message) {
      // TODO: copy message
      __applyPAP__(badCB, 1, message);
    })
    .onSuccess([goodCB]() {
      __applyPAP__(goodCB, 1, NULL);
    });
}

// TODO: Add flags
void madamqp__declareExchange(AMQP::TcpChannel *channel, char *name, AMQP::ExchangeType type, PAP_t *badCB, PAP_t *goodCB) {
  channel->declareExchange(name, type)
    .onError([badCB](const char *message) {
      // TODO: copy message
      __applyPAP__(badCB, 1, message);
    })
    .onSuccess([goodCB]() {
      __applyPAP__(goodCB, 1, NULL);
    });
}

AMQP::ExchangeType madamqp__makeFanout() {
  return AMQP::fanout;
}

AMQP::ExchangeType madamqp__makeDirect() {
  return AMQP::fanout;
}

AMQP::ExchangeType madamqp__makeTopic() {
  return AMQP::fanout;
}

AMQP::ExchangeType madamqp__makeHeaders() {
  return AMQP::fanout;
}

void madamqp__declareQueue(AMQP::TcpChannel *channel, char *name, PAP_t *badCB, PAP_t *goodCB) {
  channel->declareQueue(name)
    .onError([badCB](const char *message) {
      // TODO: copy message
      __applyPAP__(badCB, 1, message);
    })
    .onSuccess([goodCB]() {
      __applyPAP__(goodCB, 1, NULL);
    });
}

void madamqp__publish(AMQP::TcpChannel *channel, char *exchange, char *routingKey, char *message) {
  channel->publish(exchange, routingKey, message);
}


#ifdef __cplusplus
}
#endif
