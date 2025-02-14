/**
 *  @file
 *  @copyright defined in bcc/LICENSE
 */
#pragma once
#include <bccio/chain/database_utils.hpp>
#include <bccio/chain/authority.hpp>
#include <bccio/chain/code_object.hpp>
#include <bccio/chain/block_timestamp.hpp>
#include <bccio/chain/abi_def.hpp>

#include "multi_index_includes.hpp"

namespace bccio { namespace chain {

   class account_object : public chainbase::object<account_object_type, account_object> {
      OBJECT_CTOR(account_object,(abi))

      id_type              id;
      account_name         name; //< name should not be changed within a chainbase modifier lambda
      block_timestamp_type creation_date;
      shared_blob          abi;

      void set_abi( const bccio::chain::abi_def& a ) {
         abi.resize( fc::raw::pack_size( a ) );
         fc::datastream<char*> ds( abi.data(), abi.size() );
         fc::raw::pack( ds, a );
      }

      bccio::chain::abi_def get_abi()const {
         bccio::chain::abi_def a;
         BCC_ASSERT( abi.size() != 0, abi_not_found_exception, "No ABI set on account ${n}", ("n",name) );

         fc::datastream<const char*> ds( abi.data(), abi.size() );
         fc::raw::unpack( ds, a );
         return a;
      }
   };
   using account_id_type = account_object::id_type;

   struct by_name;
   using account_index = chainbase::shared_multi_index_container<
      account_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<account_object, account_object::id_type, &account_object::id>>,
         ordered_unique<tag<by_name>, member<account_object, account_name, &account_object::name>>
      >
   >;

   class account_metadata_object : public chainbase::object<account_metadata_object_type, account_metadata_object>
   {
      OBJECT_CTOR(account_metadata_object);

      enum class flags_fields : uint32_t {
         privileged = 1
      };

      id_type               id;
      account_name          name; //< name should not be changed within a chainbase modifier lambda
      uint64_t              recv_sequence = 0;
      uint64_t              auth_sequence = 0;
      uint64_t              code_sequence = 0;
      uint64_t              abi_sequence  = 0;
      digest_type           code_hash;
      time_point            last_code_update;
      uint32_t              flags = 0;
      uint8_t               vm_type = 0;
      uint8_t               vm_version = 0;

      bool is_privileged()const { return has_field( flags, flags_fields::privileged ); }

      void set_privileged( bool privileged )  {
         flags = set_field( flags, flags_fields::privileged, privileged );
      }
   };

   struct by_name;
   using account_metadata_index = chainbase::shared_multi_index_container<
      account_metadata_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<account_metadata_object, account_metadata_object::id_type, &account_metadata_object::id>>,
         ordered_unique<tag<by_name>, member<account_metadata_object, account_name, &account_metadata_object::name>>
      >
   >;

   class account_ram_correction_object : public chainbase::object<account_ram_correction_object_type, account_ram_correction_object>
   {
      OBJECT_CTOR(account_ram_correction_object);

      id_type      id;
      account_name name; //< name should not be changed within a chainbase modifier lambda
      uint64_t     ram_correction = 0;
   };

   struct by_name;
   using account_ram_correction_index = chainbase::shared_multi_index_container<
      account_ram_correction_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<account_ram_correction_object, account_ram_correction_object::id_type, &account_ram_correction_object::id>>,
         ordered_unique<tag<by_name>, member<account_ram_correction_object, account_name, &account_ram_correction_object::name>>
      >
   >;

} } // bccio::chain

CHAINBASE_SET_INDEX_TYPE(bccio::chain::account_object, bccio::chain::account_index)
CHAINBASE_SET_INDEX_TYPE(bccio::chain::account_metadata_object, bccio::chain::account_metadata_index)
CHAINBASE_SET_INDEX_TYPE(bccio::chain::account_ram_correction_object, bccio::chain::account_ram_correction_index)


FC_REFLECT(bccio::chain::account_object, (name)(creation_date)(abi))
FC_REFLECT(bccio::chain::account_metadata_object, (name)(recv_sequence)(auth_sequence)(code_sequence)(abi_sequence)
                                                  (code_hash)(last_code_update)(flags)(vm_type)(vm_version))
FC_REFLECT(bccio::chain::account_ram_correction_object, (name)(ram_correction))
