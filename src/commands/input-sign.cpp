/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-explorer.
 *
 * libbitcoin-explorer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <bitcoin/explorer/commands/input-sign.hpp>

#include <iostream>
#include <cstdint>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/explorer/primitives/base16.hpp>
#include <bitcoin/explorer/utility/utility.hpp>

using namespace bc;
using namespace bc::explorer;
using namespace bc::explorer::commands;
using namespace bc::explorer::primitives;

// The BX_INPUT_SIGN_FAILED condition uncovered by test.
// This is because a vector to produce the failure is not known.
console_result input_sign::invoke(std::ostream& output, std::ostream& error)
{
    // Bound parameters.
    const auto index = get_index_option();
    const auto hash_type = get_sign_type_option();
    const tx_type& tx = get_transaction_argument();
    const ec_secret& private_key = get_ec_private_key_argument();
    const script_type& prevout_script = get_prevout_script_argument();
    const data_chunk& nonce = get_nonce_option();

    const auto deterministic = nonce.empty();
    if (!deterministic && nonce.size() < minimum_seed_size)
    {
        error << BX_INPUT_SIGN_SHORT_NONCE << std::endl;
        return console_result::failure;
    }

    if (index >= tx.inputs.size())
    {
        error << BX_INPUT_SIGN_INDEX_OUT_OF_RANGE << std::endl;
        return console_result::failure;
    }

    bool success;
    data_chunk signature;

    if (deterministic)
        success = create_signature(signature, private_key, prevout_script, tx,
            index, hash_type);
    else
        success = create_signature(signature, private_key, prevout_script, tx,
            index, hash_type, new_key(nonce));

    if (!success)
    {
        error << BX_INPUT_SIGN_FAILED << std::endl;
        return console_result::failure;
    }

    output << base16(signature) << std::endl;
    return console_result::okay;
}
