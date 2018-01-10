//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "td/actor/actor.h"
#include "td/actor/PromiseFuture.h"

#include "td/telegram/net/NetQuery.h"

#include "td/telegram/td_api.h"

#include "td/utils/common.h"

#include <array>

namespace td {

class DeviceTokenManager : public NetQueryCallback {
 public:
  explicit DeviceTokenManager(ActorShared<> parent) : parent_(std::move(parent)) {
  }
  void register_device(tl_object_ptr<td_api::DeviceToken> device_token_ptr, vector<int32> other_user_ids,
                       Promise<tl_object_ptr<td_api::ok>> promise);

 private:
  static constexpr size_t MAX_OTHER_USER_IDS = 100;

  ActorShared<> parent_;
  enum TokenType : int32 { APNS = 1, GCM = 2, MPNS = 3, SimplePush = 4, UbuntuPhone = 5, Blackberry = 6, Size };
  struct TokenInfo {
    enum class State { Sync, Unregister, Register };
    State state = State::Sync;
    string token;
    uint64 net_query_id = 0;
    vector<int32> other_user_ids;
    bool is_app_sandbox = false;
    Promise<tl_object_ptr<td_api::ok>> promise;

    template <class StorerT>
    void store(StorerT &storer) const;

    template <class ParserT>
    void parse(ParserT &parser);
  };

  friend StringBuilder &operator<<(StringBuilder &string_builder, const TokenInfo &token_info);

  std::array<TokenInfo, TokenType::Size> tokens_;
  int32 sync_cnt_{0};

  void start_up() override;

  static string get_database_key(int32 token_type);
  void save_info(int32 token_type);

  void dec_sync_cnt();

  void loop() override;
  void on_result(NetQueryPtr net_query) override;
};

}  // namespace td
