#pragma once

#include <string>
#include <string_view>

namespace zzwlib {
    class base_error {
    public:
        enum class type : int {
            ok = 0,
            op_not_permitted = 1,
            no_file_or_dir = 2,
            no_process = 3,
            int_sys_call = 4,
            io_error = 5,
            no_device_or_addr = 6,
            arg_list_too_long = 7,
            exec_format_err = 8,
            bad_file_desc = 9,
            no_child_proc = 10,
            res_temp_unavail = 11,
            cannot_alloc_mem = 12,
            perm_denied = 13,
            bad_addr = 14,
            block_dev_req = 15,
            dev_or_res_busy = 16,
            file_exists = 17,
            inv_cross_dev_link = 18,
            no_device = 19,
            not_dir = 20,
            is_dir = 21,
            inv_arg = 22,
            too_many_open_files_sys = 23,
            too_many_open_files = 24,
            inap_ioctl_for_dev = 25,
            text_file_busy = 26,
            file_too_large = 27,
            no_space_left = 28,
            illegal_seek = 29,
            read_only_fs = 30,
            too_many_links = 31,
            broken_pipe = 32,
            num_arg_out_of_domain = 33,
            num_res_out_of_range = 34,
            res_deadlock_avoided = 35,
            file_name_too_long = 36,
            no_locks_avail = 37,
            func_not_impl = 38,
            dir_not_empty = 39,
            too_many_sym_links = 40,
            unknown_err_41 = 41,
            no_msg_of_type = 42,
            id_removed = 43,
            ch_num_out_of_range = 44,
            lvl_2_not_sync = 45,
            lvl_3_halted = 46,
            lvl_3_reset = 47,
            link_num_out_of_range = 48,
            proto_driver_not_attached = 49,
            no_csi_struct_avail = 50,
            lvl_2_halted_again = 51,
            inv_exchange = 52,
            inv_req_desc = 53,
            exchange_full = 54,
            no_anode = 55,
            inv_req_code = 56,
            inv_slot = 57,
            unknown_err_58 = 58,
            bad_font_format = 59,
            dev_not_stream = 60,
            no_data_avail = 61,
            timer_expired = 62,
            out_of_streams_res = 63,
            machine_not_on_net = 64,
            package_not_installed = 65,
            obj_is_remote = 66,
            link_severed = 67,
            advertise_err = 68,
            srmount_err = 69,
            comm_err_on_send = 70,
            proto_err = 71,
            multihop_attmpted = 72,
            rfs_specific_err = 73,
            bad_msg = 74,
            val_too_large = 75,
            name_not_unique = 76,
            file_desc_bad_state = 77,
            remote_addr_changed = 78,
            cannot_access_lib = 79,
            accessing_corrupted_lib = 80,
            lib_section_corrupted = 81,
            too_many_shared_libs = 82,
            cannot_exec_shared_lib = 83,
            inv_multibyte_char = 84,
            int_sys_call_restart = 85,
            streams_pipe_err = 86,
            too_many_users = 87,
            socket_op_non_socket = 88,
            dest_addr_req = 89,
            msg_too_long = 90,
            proto_wrong_type = 91,
            proto_not_avail = 92,
            proto_not_supported = 93,
            socket_type_not_supported = 94,
            op_not_supported = 95,
            proto_family_not_supported = 96,
            addr_family_not_supported = 97,
            addr_in_use = 98,
            cannot_assign_addr = 99,
            network_down = 100,
            network_unreachable = 101,
            network_drop_conn = 102,
            software_abort_conn = 103,
            conn_reset_by_peer = 104,
            no_buffer_space = 105,
            transport_endpoint_connected = 106,
            transport_endpoint_not_connected = 107,
            cannot_send_after_shutdown = 108,
            too_many_refs_cannot_splice = 109,
            conn_timeout = 110,
            conn_refused = 111,
            host_down = 112,
            no_route_to_host = 113,
            op_in_progress = 114,
            op_now_in_progress = 115,
            stale_file_handle = 116,
            struct_needs_cleaning = 117,
            not_xenix_named_type_file = 118,
            no_xenix_semaphores = 119,
            is_named_type_file = 120,
            remote_io_error = 121,
            disk_quota_exceeded = 122,
            no_medium_found = 123,
            wrong_medium_type = 124,
            op_canceled = 125,
            req_key_not_avail = 126,
            key_expired = 127,
            key_revoked = 128,
            key_rejected_by_service = 129,
            owner_died = 130,
            state_not_recoverable = 131,
            op_not_possible_due_to_rf_kill = 132,
            mem_page_hardware_error = 133
        };

        static std::string_view to_string(type err) noexcept {
            switch (err) {
            case type::ok:
                return "ok";
            case type::op_not_permitted:
                return "Operation not permitted";
            case type::no_file_or_dir:
                return "No such file or directory";
            case type::no_process:
                return "No such process";
            case type::int_sys_call:
                return "Interrupted system call";
            case type::io_error:
                return "I/O error";
            case type::no_device_or_addr:
                return "No such device or address";
            case type::arg_list_too_long:
                return "Argument list too long";
            case type::exec_format_err:
                return "Exec format error";
            case type::bad_file_desc:
                return "Bad file descriptor";
            case type::no_child_proc:
                return "No child processes";
            case type::res_temp_unavail:
                return "Resource temporarily unavailable";
            case type::cannot_alloc_mem:
                return "Cannot allocate memory";
            case type::perm_denied:
                return "Permission denied";
            case type::bad_addr:
                return "Bad address";
            case type::block_dev_req:
                return "Block device required";
            case type::dev_or_res_busy:
                return "Device or resource busy";
            case type::file_exists:
                return "File exists";
            case type::inv_cross_dev_link:
                return "Invalid cross-device link";
            case type::no_device:
                return "No such device";
            case type::not_dir:
                return "Not a directory";
            case type::is_dir:
                return "Is a directory";
            case type::inv_arg:
                return "Invalid argument";
            case type::too_many_open_files_sys:
                return "Too many open files in system";
            case type::too_many_open_files:
                return "Too many open files";
            case type::inap_ioctl_for_dev:
                return "Inappropriate I/O control operation";
            case type::text_file_busy:
                return "Text file busy";
            case type::file_too_large:
                return "File too large";
            case type::no_space_left:
                return "No space left on device";
            case type::illegal_seek:
                return "Illegal seek";
            case type::read_only_fs:
                return "Read-only file system";
            case type::too_many_links:
                return "Too many links";
            case type::broken_pipe:
                return "Broken pipe";
            case type::num_arg_out_of_domain:
                return "Numerical argument out of domain";
            case type::num_res_out_of_range:
                return "Numerical result out of range";
            case type::res_deadlock_avoided:
                return "Resource deadlock avoided";
            case type::file_name_too_long:
                return "File name too long";
            case type::no_locks_avail:
                return "No locks available";
            case type::func_not_impl:
                return "Function not implemented";
            case type::dir_not_empty:
                return "Directory not empty";
            case type::too_many_sym_links:
                return "Too many symbolic links";
            case type::unknown_err_41:
                return "Unknown error 41";
            case type::no_msg_of_type:
                return "No message of desired type";
            case type::id_removed:
                return "Identifier removed";
            case type::ch_num_out_of_range:
                return "Channel number out of range";
            case type::lvl_2_not_sync:
                return "Level 2 not synchronized";
            case type::lvl_3_halted:
                return "Level 3 halted";
            case type::lvl_3_reset:
                return "Level 3 reset";
            case type::link_num_out_of_range:
                return "Link number out of range";
            case type::proto_driver_not_attached:
                return "Protocol driver not attached";
            case type::no_csi_struct_avail:
                return "No CSI structure available";
            case type::lvl_2_halted_again:
                return "Level 2 halted again";
            case type::inv_exchange:
                return "Invalid exchange";
            case type::inv_req_desc:
                return "Invalid request descriptor";
            case type::exchange_full:
                return "Exchange full";
            case type::no_anode:
                return "No anode";
            case type::inv_req_code:
                return "Invalid request code";
            case type::inv_slot:
                return "Invalid slot";
            case type::unknown_err_58:
                return "Unknown error 58";
            case type::bad_font_format:
                return "Bad font file format";
            case type::dev_not_stream:
                return "Device not a stream";
            case type::no_data_avail:
                return "No data available";
            case type::timer_expired:
                return "Timer expired";
            case type::out_of_streams_res:
                return "Out of streams resources";
            case type::machine_not_on_net:
                return "Machine is not on the network";
            case type::package_not_installed:
                return "Package not installed";
            case type::obj_is_remote:
                return "Object is remote";
            case type::link_severed:
                return "Link has been severed";
            case type::advertise_err:
                return "Advertise error";
            case type::srmount_err:
                return "Srmount error";
            case type::comm_err_on_send:
                return "Communication error on send";
            case type::proto_err:
                return "Protocol error";
            case type::multihop_attmpted:
                return "Multihop attempted";
            case type::rfs_specific_err:
                return "RFS specific error";
            case type::bad_msg:
                return "Bad message";
            case type::val_too_large:
                return "Value too large";
            case type::name_not_unique:
                return "Name not unique on network";
            case type::file_desc_bad_state:
                return "File descriptor in bad state";
            case type::remote_addr_changed:
                return "Remote address changed";
            case type::cannot_access_lib:
                return "Cannot access a required shared library";
            case type::accessing_corrupted_lib:
                return "Accessing a corrupted shared library";
            case type::lib_section_corrupted:
                return "Corrupted shared library section";
            case type::too_many_shared_libs:
                return "Too many shared libraries";
            case type::cannot_exec_shared_lib:
                return "Cannot exec a shared library directly";
            case type::inv_multibyte_char:
                return "Invalid multibyte character";
            case type::int_sys_call_restart:
                return "Interrupted system call should be restarted";
            case type::streams_pipe_err:
                return "Streams pipe error";
            case type::too_many_users:
                return "Too many users";
            case type::socket_op_non_socket:
                return "Socket operation on non-socket";
            case type::dest_addr_req:
                return "Destination address required";
            case type::msg_too_long:
                return "Message too long";
            case type::proto_wrong_type:
                return "Protocol wrong type for socket";
            case type::proto_not_avail:
                return "Protocol not available";
            case type::proto_not_supported:
                return "Protocol not supported";
            case type::socket_type_not_supported:
                return "Socket type not supported";
            case type::op_not_supported:
                return "Operation not supported";
            case type::proto_family_not_supported:
                return "Protocol family not supported";
            case type::addr_family_not_supported:
                return "Address family not supported";
            case type::addr_in_use:
                return "Address already in use";
            case type::cannot_assign_addr:
                return "Cannot assign requested address";
            case type::network_down:
                return "Network is down";
            case type::network_unreachable:
                return "Network is unreachable";
            case type::network_drop_conn:
                return "Network dropped connection on reset";
            case type::software_abort_conn:
                return "Software caused connection abort";
            case type::conn_reset_by_peer:
                return "Connection reset by peer";
            case type::no_buffer_space:
                return "No buffer space available";
            case type::transport_endpoint_connected:
                return "Transport endpoint is already connected";
            case type::transport_endpoint_not_connected:
                return "Transport endpoint is not connected";
            case type::cannot_send_after_shutdown:
                return "Cannot send after transport endpoint shutdown";
            case type::too_many_refs_cannot_splice:
                return "Too many references: cannot splice";
            case type::conn_timeout:
                return "Connection timed out";
            case type::conn_refused:
                return "Connection refused";
            case type::host_down:
                return "Host is down";
            case type::no_route_to_host:
                return "No route to host";
            case type::op_in_progress:
                return "Operation in progress";
            case type::op_now_in_progress:
                return "Operation now in progress";
            case type::stale_file_handle:
                return "Stale file handle";
            case type::struct_needs_cleaning:
                return "Structure needs cleaning";
            case type::not_xenix_named_type_file:
                return "Not a XENIX named type file";
            case type::no_xenix_semaphores:
                return "No XENIX semaphores available";
            case type::is_named_type_file:
                return "Is a named type file";
            case type::remote_io_error:
                return "Remote I/O error";
            case type::disk_quota_exceeded:
                return "Disk quota exceeded";
            case type::no_medium_found:
                return "No medium found";
            case type::wrong_medium_type:
                return "Wrong medium type";
            case type::op_canceled:
                return "Operation canceled";
            case type::req_key_not_avail:
                return "Required key not available";
            case type::key_expired:
                return "Key has expired";
            case type::key_revoked:
                return "Key has been revoked";
            case type::key_rejected_by_service:
                return "Key was rejected by service";
            case type::owner_died:
                return "Owner died";
            case type::state_not_recoverable:
                return "State not recoverable";
            case type::op_not_possible_due_to_rf_kill:
                return "Operation not possible due to RF-kill";
            case type::mem_page_hardware_error:
                return "Memory page hardware error";
            }
            return "unknown";
        }
    };
};
