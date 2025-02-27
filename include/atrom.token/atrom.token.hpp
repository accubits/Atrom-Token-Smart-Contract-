#pragma once

#include <atrom/asset.hpp>
#include <atrom/atrom.hpp>

#include <string>

namespace atromsystem {
   class system_contract;
}

namespace atrom {

   using std::string;

   /**
    * atrom.token contract defines the structures and actions that allow users to create, issue, and manage
    * tokens on atrom based blockchains.
    */
   class [[atrom::contract("atrom.token")]] token : public contract {
      public:
         using contract::contract;

         /**
          * Allows `issuer` account to create a token in supply of `maximum_supply`. If validation is successful a new entry in statstable for token symbol scope gets created.
          *
          * @param issuer - the account that creates the token,
          * @param maximum_supply - the maximum supply set for the token created.
          *
          * @pre Token symbol has to be valid,
          * @pre Token symbol must not be already created,
          * @pre maximum_supply has to be smaller than the maximum supply allowed by the system: 1^62 - 1.
          * @pre Maximum supply must be positive;
          */
         [[atrom::action]]
         void create( const name&   issuer,
                      const asset&  maximum_supply);
          /**
          *  This action issues to `to` account a `quantity` of tokens.
          *
          * @param to - the account to issue tokens to, it must be the same as the issuer,
          * @param quntity - the amount of tokens to be issued,
          * @memo - the memo string that accompanies the token issue transaction.
          */
         [[atrom::action]]
         void issue( const name& to, const asset& quantity, const string& memo );

          /**
          *  This action issues to `to` account a `quantity` of tokens.
          *
          * @param admin_user- the account to assign admin,
          * @param quntity - the amount of tokens to be issued,
          */
         [[atrom::action]]
         void admin_create( const name& admin_user, const asset& quantity);

          /**
          *  This action issues to `to` account a `quantity` of tokens.
          *
          * @param old_admin_user - the account of current admin user.
          * @param new_admin_user - the account of new admin user.
          * @param quntity - the amount of tokens to be issued,
          */
         [[atrom::action]]
         void admin_update( const name& old_admin_user ,  const name& old_admin_user , const asset& quantity );

       /**
          * Allows `from` account to transfer to `to` account the `quantity` tokens.
          * One account is debited and the other is credited with quantity tokens.
          *
          * @param from - the account to transfer from,
          * @param to - the account to be transferred to,
          * @param quantity - the quantity of tokens to be transferred,
          * @param memo - the memo string to accompany the transaction.
          */
         [[atrom::action]]
         void transfer_admin( const name&    from,
                        const name&    to,
                        const asset&   quantity,
                        const string&  memo );

         /**
          * The opposite for create action, if all validations succeed,
          * it debits the statstable.supply amount.
          *
          * @param quantity - the quantity of tokens to retire,
          * @param memo - the memo string to accompany the transaction.
          */
         [[atrom::action]]
         void retire( const asset& quantity, const string& memo );

         /**
          * Allows `from` account to transfer to `to` account the `quantity` tokens.
          * One account is debited and the other is credited with quantity tokens.
          *
          * @param from - the account to transfer from,
          * @param to - the account to be transferred to,
          * @param quantity - the quantity of tokens to be transferred,
          * @param memo - the memo string to accompany the transaction.
          */
         [[atrom::action]]
         void transfer( const name&    from,
                        const name&    to,
                        const asset&   quantity,
                        const string&  memo );
         /**
          * Allows `ram_payer` to create an account `owner` with zero balance for
          * token `symbol` at the expense of `ram_payer`.
          *
          * @param owner - the account to be created,
          * @param symbol - the token to be payed with by `ram_payer`,
          * @param ram_payer - the account that supports the cost of this action.
          *
          * More information can be read [here](https://github.com/atrom/atrom.contracts/issues/62)
          * and [here](https://github.com/atrom/atrom.contracts/issues/61).
          */
         [[atrom::action]]
         void open( const name& owner, const symbol& symbol, const name& ram_payer );

         /**
          * This action is the opposite for open, it closes the account `owner`
          * for token `symbol`.
          *
          * @param owner - the owner account to execute the close action for,
          * @param symbol - the symbol of the token to execute the close action for.
          *
          * @pre The pair of owner plus symbol has to exist otherwise no action is executed,
          * @pre If the pair of owner plus symbol exists, the balance has to be zero.
          */
         [[atrom::action]]
         void close( const name& owner, const symbol& symbol );

         static asset get_supply( const name& token_contract_account, const symbol_code& sym_code )
         {
            stats statstable( token_contract_account, sym_code.raw() );
            const auto& st = statstable.get( sym_code.raw() );
            return st.supply;
         }

         static asset get_balance( const name& token_contract_account, const name& owner, const symbol_code& sym_code )
         {
            accounts accountstable( token_contract_account, owner.value );
            const auto& ac = accountstable.get( sym_code.raw() );
            return ac.balance;
         }

         using create_action = atrom::action_wrapper<"create"_n, &token::create>;
         using admin_create_action = atrom::action_wrapper<"create"_n, &token::create>;
         using admin_update_action = atrom::action_wrapper<"create"_n, &token::create>;
         using transfer_admin_action = atrom::action_wrapper<"create"_n, &token::create>;
         using issue_action = atrom::action_wrapper<"issue"_n, &token::issue>;
         using retire_action = atrom::action_wrapper<"retire"_n, &token::retire>;
         using transfer_action = atrom::action_wrapper<"transfer"_n, &token::transfer>;
         using open_action = atrom::action_wrapper<"open"_n, &token::open>;
         using close_action = atrom::action_wrapper<"close"_n, &token::close>;
      private:
         struct [[atrom::table]] account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
         };

         struct [[atrom::table]] currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
         };
         
         /**
         Create a table for storing admin details
         **/
         struct [[atrom::table]] admin_info {
            asset    balance;
            name     admin;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
         };

         typedef atrom::multi_index< "accounts"_n, account > accounts;
         typedef atrom::multi_index< "stat"_n, currency_stats > stats;
         typedef atrom::multi_index< "admin"_n, admin_info > admins;


         void sub_balance( const name& owner, const asset& value );
         void add_balance( const name& owner, const asset& value, const name& ram_payer );
   };

}
