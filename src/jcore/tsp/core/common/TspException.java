/* 
 * $Id: TspException.java,v 1.2 2004-11-02 05:11:41 sgalles Exp $
 * -----------------------------------------------------------------------
 * 
 * TSP Library - core components for a generic Transport Sampling Protocol.
 * 
 * Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * -----------------------------------------------------------------------
 * 
 * Project    : TSP
 * SubProject : jTSP
 * Maintainer : tsp@astrium-space.com
 * Component  : Consumer
 * 
 * -----------------------------------------------------------------------
 * 
 * Purpose   : 
 * 
 * -----------------------------------------------------------------------
 */

package tsp.core.common;

/**
 * The class <code>TspException</code> indicates
 *
 * @see java.lang.Exception
 */
public class TspException extends Exception {

    /**
     * Constructs an <code>TspException</code> with a reason of
     * {@link TspException#RPC_FAILED}.
     */
    public TspException() {
	super();
        
    }

    /**
     * Constructs an <code>TspException</code> with the specified detail
     * message.
     *
     * @param  s  The detail message.
     */
    public TspException(String s) {
        super(s);

//         reason = RPC_FAILED;
//         message = s;
    }

    /**
     * Constructs an <code>TspException</code> with the specified detail
     * reason and message. For possible reasons, see below.
     *
     * @param  r  The detail reason.
     * @param  s  The detail message.
     */
    public TspException(int r, String s) {
        super();

        reason = r;
        message = s;
    }

    /**
     * Constructs an <code>TspException</code> with the specified detail
     * reason. The detail message is derived automatically from the reason.
     *
     * @param  r  The reason. This can be one of the constants -- oops, that
     *   should be "public final static integers" -- defined in this
     *   interface.
     */
    public TspException(int r) {
        super();

        reason = r;
        switch ( r ) {

        case RPC_FAILED:
            message = "ONC/RPC generic failure";
            break;
        case RPC_BUFFEROVERFLOW:
            message = "ONC/RPC buffer overflow";
            break;
        case RPC_BUFFERUNDERFLOW:
            message = "ONC/RPC buffer underflow";
            break;
        case RPC_WRONGMESSAGE:
            message = "wrong ONC/RPC message type received";
            break;

        case TSP_SUCCESS:
        default:
            break;
        }
    }

   

    /**
     * Returns the error reason of this ONC/RPC exception object.
     *
     * @return  The error reason of this <code>TspException</code> object if
     *   it was {@link #TspException(int) created} with an error reason; or
     *   <code>RPC_FAILED</code> if it was {@link #TspException() created}
     *   with no error reason.
     */
    public int getReason() {
        return reason;
    }

    /**
     * The TSP operation has been successfully completed.
     */
    public static final int TSP_SUCCESS = 0;
    /**
     * The TSP consumer session is in the broken state.
     */
    public static final int CONSUMER_SESSION_BROKEN = 100;
    /**
     * The client can not decode the result from the remote procedure call.
     */
    public static final int CONSUMER_REQUEST_FAILED = 2;
    /**
     * Encoded information can not be sent.
     */
    public static final int RPC_CANTSEND = 3;
    /**
     * Information to be decoded can not be received.
     */
    public static final int RPC_CANTRECV = 4;
    /**
     * The remote procedure call timed out.
     */
    public static final int RPC_TIMEDOUT = 5;
    /**
     * ONC/RPC versions of server and client are not compatible.
     */
    public static final int RPC_VERSMISMATCH = 6;
    /**
     * The ONC/RPC server did not accept the authentication sent by the
     * client. Bad girl/guy!
     */
    public static final int RPC_AUTHERROR = 7;
    /**
     * The ONC/RPC server does not support this particular program.
     */
    public static final int RPC_PROGUNAVAIL = 8;
    /**
     * The ONC/RPC server does not support this particular version of the
     * program.
     */
    public static final int RPC_PROGVERSMISMATCH = 9;
    /**
     * The given procedure is not available at the ONC/RPC server.
     */
    public static final int RPC_PROCUNAVAIL = 10;
    /**
     * The ONC/RPC server could not decode the arguments sent within the
     * call message.
     */
    public static final int RPC_CANTDECODEARGS = 11;
    /**
     * The ONC/RPC server encountered a system error and thus was not able
     * to carry out the requested remote function call successfully.
     */
    public static final int RPC_SYSTEMERROR = 12;
    /**
     * The caller specified an unknown/unsupported IP protocol. Currently,
     * only {@link OncRpcProtocols#ONCRPC_TCP} and
     * {@link OncRpcProtocols#ONCRPC_UDP} are supported.
     */
    public static final int RPC_UNKNOWNPROTO = 17;
    /**
     * The portmapper could not be contacted at the given host.
     */
    public static final int RPC_PMAPFAILURE = 14;
    /**
     * The requested program is not registered with the given host.
     */
    public static final int RPC_PROGNOTREGISTERED = 15;
    /**
     * A generic ONC/RPC exception occured. Shit happens...
     */
    public static final int RPC_FAILED = 16;
    /**
     * A buffer overflow occured with an encoding XDR stream. This happens
     * if you use UDP-based (datagram-based) XDR streams and you try to encode
     * more data than can fit into the sending buffers.
     */
    public static final int RPC_BUFFEROVERFLOW = 42;
    /**
     * A buffer underflow occured with an decoding XDR stream. This happens
     * if you try to decode more data than was sent by the other communication
     * partner.
     */
    public static final int RPC_BUFFERUNDERFLOW = 43;
    /**
     * Either a ONC/RPC server or client received the wrong type of ONC/RPC
     * message when waiting for a request or reply. Currently, only the
     * decoding methods of the classes {@link OncRpcCallMessage} and
     * {@link OncRpcReplyMessage} throw exceptions with this reason.
     */
    public static final int RPC_WRONGMESSAGE = 44;

    /**
     * Specific detail (reason) about this <code>TspException</code>,
     * like the ONC/RPC error code, as defined by the <code>RPC_xxx</code>
     * constants of this interface.
     *
     * @serial
     */
    private int reason;

    /**
     * Specific detail about this <code>TspException</code>, like a
     * detailed error message.
     *
     * @serial
     */
    private String message;

}

// End of TspException.java
